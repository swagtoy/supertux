//  $Id$
//
//  SuperTux -  A Jump'n Run
//  Copyright (C) 2003 Tobias Glaesser <tobi.web@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <config.h>

#include <typeinfo>
#include <cmath>
#include <iostream>
#include <cassert>

#include "app/globals.h"
#include "app/gettext.h"
#include "player.h"
#include "defines.h"
#include "scene.h"
#include "tile.h"
#include "special/sprite.h"
#include "sector.h"
#include "tilemap.h"
#include "camera.h"
#include "gameobjs.h"
#include "resources.h"
#include "video/screen.h"
#include "statistics.h"
#include "gameloop.h"
#include "trigger/trigger_base.h"

// behavior definitions:
#define TILES_FOR_BUTTJUMP 3
// animation times (in ms):
#define SHOOTING_TIME .150

// time before idle animation starts
#define IDLE_TIME 2.500

// growing animation
Surface* growingtux_left[GROWING_FRAMES];
Surface* growingtux_right[GROWING_FRAMES];

Surface* tux_life = 0;

Sprite* smalltux_gameover = 0;
Sprite* smalltux_star = 0;
Sprite* bigtux_star = 0;

TuxBodyParts* small_tux = 0;
TuxBodyParts* big_tux = 0;
TuxBodyParts* fire_tux = 0;
TuxBodyParts* ice_tux = 0;

PlayerKeymap keymap;

PlayerKeymap::PlayerKeymap()
{
  keymap.up    = SDLK_UP;
  keymap.down  = SDLK_DOWN;
  keymap.left  = SDLK_LEFT;
  keymap.right = SDLK_RIGHT;

  keymap.power = SDLK_LCTRL;
  keymap.jump  = SDLK_LALT;
}

void player_input_init(player_input_type* pplayer_input)
{
  pplayer_input->up = UP;
  pplayer_input->old_up = UP;
  pplayer_input->down = UP;
  pplayer_input->fire = UP;
  pplayer_input->left = UP;
  pplayer_input->old_fire = UP;
  pplayer_input->right = UP;
  pplayer_input->jump = UP;
  pplayer_input->old_jump = UP;
  pplayer_input->activate = UP;
}

void
TuxBodyParts::set_action(std::string action)
{
  if(head != NULL)
    head->set_action(action);
  if(body != NULL)
    body->set_action(action);
  if(arms != NULL)
    arms->set_action(action);
  if(feet != NULL)
    feet->set_action(action);
}

void
TuxBodyParts::one_time_animation()
{
  if(head != NULL)
    head->start_animation(1);
  if(body != NULL)
    body->start_animation(1);
  if(arms != NULL)
    arms->start_animation(1);
  if(feet != NULL)
    feet->start_animation(1);
}

void
TuxBodyParts::draw(DrawingContext& context, const Vector& pos, int layer,
                  Uint32 drawing_effect)
{
  if(head != NULL)
    head->draw(context, pos, layer-1, drawing_effect);
  if(body != NULL)
    body->draw(context, pos, layer-3, drawing_effect);
  if(arms != NULL)
    arms->draw(context, pos, layer,   drawing_effect);
  if(feet != NULL)
    feet->draw(context, pos, layer-2, drawing_effect);
}

Player::Player()
{
  init();
}

Player::~Player()
{
}

void
Player::init()
{
  holding_something = false;

  bbox.set_size(32, 32);

  size = SMALL;
  got_power = NONE_POWER;

  dir = RIGHT;
  old_dir = dir;
  duck = false;
  dead = false;

  dying   = DYING_NOT;
  last_ground_y = 0;
  fall_mode = ON_GROUND;
  jumping = false;
  flapping = false;
  can_jump = true;
  can_flap = false;
  falling_from_flap = false;
  enable_hover = false;
  butt_jump = false;
  
  flapping_velocity = 0;

  // temporary to help player's choosing a flapping
  flapping_mode = MAREK_FLAP;

  // Ricardo's flapping
  flaps_nb = 0;

  on_ground_flag = false;

  frame_main = 0;
  frame_ = 0;

  player_input_init(&input);

  frame_timer.start(.025, true);

  physic.reset();
}

int
Player::key_event(SDLKey key, int state)
{
  idle_timer.start(IDLE_TIME, true);

  if(key == keymap.right)
    {
      input.right = state;
      return true;
    }
  else if(key == keymap.left)
    {
      input.left = state;
      return true;
    }
  else if(key == keymap.up)
    {
      if(state == UP)
        input.old_up = UP;
      input.up = state;
      /* Up key also opens activates stuff */
      input.activate = state;
      return true;
    }
  else if(key == keymap.down)
    {
      input.down = state;
      return true;
    }
  else if(key == keymap.power)
    {
      if (state == UP)
        input.old_fire = UP;
      input.fire = state;

      return true;
    }
  else if(key == keymap.jump)
    {
      if (state == UP)
        input.old_jump = UP;
      input.jump = state;
      return true;
    }
  else
    return false;
}

void
Player::level_begin()
{
  move(Vector(100, 170));
  duck = false;

  dying = DYING_NOT;

  player_input_init(&input);

  on_ground_flag = false;

  physic.reset();
}

PlayerStatus&
Player::get_status()
{
  return player_status;
}

void
Player::action(float elapsed_time)
{
  if(dying && dying_timer.check()) {
    dead = true;
    return;
  }

  if (input.fire == UP)
    holding_something = false;

  /* --- HANDLE TUX! --- */
  if(dying == DYING_NOT)
    handle_input();

  movement = physic.get_movement(elapsed_time);

  if(dying == DYING_NOT) 
    {
#if 0
      collision_swept_object_map(&old_base, &base);

      if ((!invincible_timer.started() && !safe_timer.started())
          && (isspike(base.x, base.y) || isspike(base.x + base.width, base.y)
          ||  isspike(base.x, base.y + base.height)
          ||  isspike(base.x + base.width, base.y + base.height)))
      {
         kill(SHRINK);
      }

      // Don't accelerate Tux if he is running against a wall
      if (target.x != base.x)
      {
        physic.set_velocity_x(0);
      }

      // special exception for cases where we're stuck under tiles after
      // being ducked. In this case we drift out
      if(!duck && on_ground() && old_base.x == base.x && old_base.y == base.y
         && collision_object_map(base))
        {
          base.x += elapsed_time * WALK_SPEED * (dir ? 1: -1);
          previous_base = old_base = base;
        }

      // Land:
      if (!on_ground())
        {
          physic.enable_gravity(true);
          if(under_solid())
            {
              // fall down
              physic.set_velocity_y(0);
              jumped_in_solid = true;
              jumping = false;
              flapping = false;
            }
        }
      else
        {
          /* Land: */
          if (physic.get_velocity_y() < 0)
            {
              base.y = (int)(((int)base.y / 32) * 32);
              physic.set_velocity_y(0);
            }

          physic.enable_gravity(false);
          /* Reset score multiplier (for multi-hits): */
          if (!invincible_timer.started())
            {
            if(player_status.score_multiplier > player_status.max_score_multiplier)
              {
              player_status.max_score_multiplier = player_status.score_multiplier;

              // show a message
              char str[124];
              sprintf(str, _("New max combo: %d"), player_status.max_score_multiplier-1);
              Sector::current()->add_floating_text(base, str);
              }
            player_status.score_multiplier = 1;
            }
        }

      if(jumped_in_solid)
        {
          if (isbrick(base.x, base.y) ||
              isfullbox(base.x, base.y))
            {
	      Sector::current()->trygrabdistro(
                  Vector(base.x, base.y - 32), BOUNCE);
              Sector::current()->trybumpbadguy(Vector(base.x, base.y - 64));

              Sector::current()->trybreakbrick(
                  Vector(base.x, base.y), size == SMALL);

              bumpbrick(base.x, base.y);
              Sector::current()->tryemptybox(Vector(base.x, base.y), RIGHT);
            }

          if (isbrick(base.x+ 31, base.y) ||
              isfullbox(base.x+ 31, base.y))
            {
	      Sector::current()->trygrabdistro(
                  Vector(base.x+ 31, base.y - 32), BOUNCE);
              Sector::current()->trybumpbadguy(Vector(base.x+ 31, base.y - 64));

              if(size == BIG)
                Sector::current()->trybreakbrick(
                    Vector(base.x+ 31, base.y), size == SMALL);

              bumpbrick(base.x+ 31, base.y);
              Sector::current()->tryemptybox(Vector(base.x+ 31, base.y), LEFT);
            }
        }
#endif

      grabdistros();

#if 0
      if (jumped_in_solid)
        {
          ++base.y;
          ++old_base.y;
          if(on_ground())
            {
              /* Make sure jumping is off. */
              jumping = false;
              flapping = false;
            }
        }
#endif
    }

  on_ground_flag = false;
}

bool
Player::on_ground()
{
  return on_ground_flag;
}

void
Player::handle_horizontal_input()
{
  float vx = physic.get_velocity_x();
  float vy = physic.get_velocity_y();
  float ax = physic.get_acceleration_x();
  float ay = physic.get_acceleration_y();

  float dirsign = 0;
  if(input.left == DOWN && input.right == UP && (!duck || physic.get_velocity_y() != 0)) {
      old_dir = dir;
      dir = LEFT;
      dirsign = -1;
  } else if(input.left == UP && input.right == DOWN && (!duck || physic.get_velocity_y() != 0)) {
      old_dir = dir;
      dir = RIGHT;
      dirsign = 1;
  }

  if (input.fire == UP) {
      ax = dirsign * WALK_ACCELERATION_X;
      // limit speed
      if(vx >= MAX_WALK_XM && dirsign > 0) {
        vx = MAX_WALK_XM;
        ax = 0;
      } else if(vx <= -MAX_WALK_XM && dirsign < 0) {
        vx = -MAX_WALK_XM;
        ax = 0;
      }
  } else {
      ax = dirsign * RUN_ACCELERATION_X;
      // limit speed
      if(vx >= MAX_RUN_XM && dirsign > 0) {
        vx = MAX_RUN_XM;
        ax = 0;
      } else if(vx <= -MAX_RUN_XM && dirsign < 0) {
        vx = -MAX_RUN_XM;
        ax = 0;
      }
  }

  // we can reach WALK_SPEED without any acceleration
  if(dirsign != 0 && fabs(vx) < WALK_SPEED) {
    vx = dirsign * WALK_SPEED;
  }

  // changing directions?
  if(on_ground() && ((vx < 0 && dirsign >0) || (vx>0 && dirsign<0)))
    {
      // let's skid!
      if(fabs(vx)>SKID_XM && !skidding_timer.started())
        {
          skidding_timer.start(SKID_TIME);
          SoundManager::get()->play_sound(IDToSound(SND_SKID));
          // dust some partcles
          Sector::current()->add_particles(
              Vector(bbox.p1.x + (dir == RIGHT ? bbox.get_width() : 0),
                bbox.p2.y),
              dir == RIGHT ? 270+20 : 90-40, dir == RIGHT ? 270+40 : 90-20,
              Vector(280,-260), Vector(0,0.030), 3, Color(100,100,100), 3, 800,
              LAYER_OBJECTS+1);

          ax *= 2.5;
        }
      else
        {
          ax *= 2;
        }
    }

  // we get slower when not pressing any keys
  if(dirsign == 0) {
      if(fabs(vx) < WALK_SPEED) {
          vx = 0;
          ax = 0;
      } else if(vx < 0) {
          ax = WALK_ACCELERATION_X * 1.5;
      } else {
          ax = WALK_ACCELERATION_X * -1.5;
      }
  }

#if 0
  // if we're on ice slow down acceleration or deceleration
  if (isice(base.x, base.y + base.height))
  {
    /* the acceleration/deceleration rate on ice is inversely proportional to
     * the current velocity.
     */

    // increasing 1 will increase acceleration/deceleration rate
    // decreasing 1 will decrease acceleration/deceleration rate
    //  must stay above zero, though
    if (ax != 0) ax *= 1 / fabs(vx);
  }
#endif

  physic.set_velocity(vx, vy);
  physic.set_acceleration(ax, ay);
}

void
Player::handle_vertical_input()
{
  // set fall mode...
  if(on_ground()) {
    fall_mode = ON_GROUND;
    last_ground_y = get_pos().y;
  } else {
    if(get_pos().y > last_ground_y)
      fall_mode = FALLING;
    else if(fall_mode == ON_GROUND)
      fall_mode = JUMPING;
  }

  // Press jump key
  if(input.jump == DOWN && can_jump && on_ground())
    {
      if(duck) { // only jump a little bit when in duck mode {
        physic.set_velocity_y(300);
      } else {
        // jump higher if we are running
        if (fabs(physic.get_velocity_x()) > MAX_WALK_XM)
          physic.set_velocity_y(580);
        else
          physic.set_velocity_y(520);
      }

      //bbox.move(Vector(0, -1));
      jumping = true;
      flapping = false;
      can_jump = false;
      can_flap = false;
      flaps_nb = 0; // Ricardo's flapping
      if (size == SMALL)
        SoundManager::get()->play_sound(IDToSound(SND_JUMP));
      else
        SoundManager::get()->play_sound(IDToSound(SND_BIGJUMP));
    }
  // Let go of jump key
  else if(input.jump == UP)
    {
      if (!flapping && !duck && !falling_from_flap && !on_ground())
         {
            can_flap = true;
         }
      if (jumping && physic.get_velocity_y() > 0)
         {
            jumping = false;
            physic.set_velocity_y(0);
         }
    }

 // temporary to help player's choosing a flapping
 if(flapping_mode == RICARDO_FLAP)
   {
   // Flapping, Ricardo's version
   // similar to SM3 Fox
   if(input.jump == DOWN && input.old_jump == UP && can_flap &&
     flaps_nb < 3)
     {
       physic.set_velocity_y(350);
       physic.set_velocity_x(physic.get_velocity_x() * 35);
       flaps_nb++;
     }
   }
  else if(flapping_mode == MAREK_FLAP)
   {
   // Flapping, Marek's version
   if (input.jump == DOWN && can_flap)
     {
         if (!flapping_timer.started())
            {
               flapping_timer.start(TUX_FLAPPING_TIME);
               flapping_velocity = physic.get_velocity_x();
            }
         if (flapping_timer.check()) 
            {
               can_flap = false;
               falling_from_flap = true;
            }
         jumping = true;
         flapping = true;
         if (!flapping_timer.check()) {
           float cv = flapping_velocity * sqrt(
               TUX_FLAPPING_TIME - flapping_timer.get_timegone() 
               / TUX_FLAPPING_TIME);

           //Handle change of direction while flapping
           if (((dir == LEFT) && (cv > 0)) || (dir == RIGHT) && (cv < 0)) {
             cv *= (-1);
           }
           physic.set_velocity_x(cv);
           physic.set_velocity_y(
               flapping_timer.get_timegone()/.850);
         }
     }
   }
  else if(flapping_mode == RYAN_FLAP)
   {
   // Flapping, Ryan's version
   if (input.jump == DOWN && can_flap)
     {
         if (!flapping_timer.started())
            {
               flapping_timer.start(TUX_FLAPPING_TIME);
            }
         if (flapping_timer.check()) 
            {
               can_flap = false;
               falling_from_flap = true;
            }
         jumping = true;
         flapping = true;
         if (flapping && flapping_timer.get_timegone() <= TUX_FLAPPING_TIME
                && physic.get_velocity_y() < 0)
            {
               float gravity = Sector::current()->gravity;
               (void)gravity;
               float xr = (fabsf(physic.get_velocity_x()) / MAX_RUN_XM);

               // XXX: magic numbers. should be a percent of gravity
               //      gravity is (by default) -0.1f
               physic.set_acceleration_y(12 + 1*xr);

#if 0
               // To slow down x-vel when flapping (not working)
               if (fabsf(physic.get_velocity_x()) > MAX_WALK_XM)
               {
                   if (physic.get_velocity_x() < 0)
                       physic.set_acceleration_x(1.0f);
                   else if (physic.get_velocity_x() > 0)
                       physic.set_acceleration_x(-1.0f);
               }
#endif
            }
     }
    else
     {
        physic.set_acceleration_y(0);
     }
   }

   // Hover
   //(disabled by default, use cheat code "hover" to toggle on/off)
   //TODO: needs some tweaking, especially when used together with double jump and jumping off badguys
   if (enable_hover && input.jump == DOWN && !jumping && !butt_jump && physic.get_velocity_y() <= 0)
      {
         physic.set_velocity_y(-100);
      }

#if 0
   /* In case the player has pressed Down while in a certain range of air,
      enable butt jump action */
  if (input.down == DOWN && !butt_jump && !duck)
    if(tiles_on_air(TILES_FOR_BUTTJUMP) && jumping)
      butt_jump = true;
#endif

   /* When Down is not held anymore, disable butt jump */
  if(butt_jump && input.down == UP)
    butt_jump = false;

  // Do butt jump
  if (butt_jump && on_ground() && size == BIG)
  {
    // Add a smoke cloud
    if (duck) 
      Sector::current()->add_smoke_cloud(Vector(get_pos().x - 32, get_pos().y));
    else 
      Sector::current()->add_smoke_cloud(
          Vector(get_pos().x - 32, get_pos().y + 32));
    
    butt_jump = false;

#if 0
    // Break bricks beneath Tux
    if(Sector::current()->trybreakbrick(
          Vector(base.x + 1, base.y + base.height), false)
        || Sector::current()->trybreakbrick(
           Vector(base.x + base.width - 1, base.y + base.height), false))
    {
      physic.set_velocity_y(2);
      butt_jump = true;
    }
#endif

#if 0
    // Kill nearby badguys
    std::vector<GameObject*> gameobjects = Sector::current()->gameobjects;
    for (std::vector<GameObject*>::iterator i = gameobjects.begin();
         i != gameobjects.end();
         i++)
    {
      BadGuy* badguy = dynamic_cast<BadGuy*> (*i);
      if(badguy)
      {
        // don't kill when badguys are already dying or in a certain mode
        if(badguy->dying == DYING_NOT && badguy->mode != BadGuy::BOMB_TICKING &&
           badguy->mode != BadGuy::BOMB_EXPLODE)
          {
            if (fabsf(base.x - badguy->base.x) < 96 &&
                fabsf(base.y - badguy->base.y) < 64)
              badguy->kill_me(25);
          }
      }
    }
#endif
  }

  /** jumping is only allowed if we're about to touch ground soon and if the
   * button has been up in between the last jump
   */
  // FIXME
#if 0
  if ( (issolid(get_pos().x + bbox.get_width() / 2,
          get_pos().y + bbox.get_height() + 64) ||
        issolid(get_pos().x + 1, get_pos().y + bbox.get_height() + 64) ||
        issolid(get_pos().x + bbox.get_width() - 1,
          get_pos().y + bbox.get_height() + 64))
       && jumping  == false
       && can_jump == false
       && input.jump == DOWN
       && input.old_jump == UP)
    {
      can_jump = true;
    }
#endif

  if(on_ground())   /* Make sure jumping is off. */
    {
      jumping = false;
      flapping = false;
      falling_from_flap = false;
      if (flapping_timer.started()) {
        flapping_timer.start(0);
      }

      physic.set_acceleration_y(0); //for flapping
    }

  input.old_jump = input.jump;
}

void
Player::handle_input()
{
  /* Handle horizontal movement: */
    handle_horizontal_input();

  /* Jump/jumping? */

  if (on_ground() && input.jump == UP)
    can_jump = true;
  handle_vertical_input();

  /* Shoot! */
  if (input.fire == DOWN && input.old_fire == UP && got_power != NONE_POWER)
    {
      if(Sector::current()->add_bullet(
            get_pos() + Vector(0, bbox.get_height()/2),
          physic.get_velocity_x(), dir))
        shooting_timer.start(SHOOTING_TIME);
      input.old_fire = DOWN;
    }

  /* tux animations: */
  if(frame_timer.check()) {
    if (input.right == UP && input.left == UP)
    {
      frame_main = 1;
      frame_ = 1;
    }
    else
    {
      if ((input.fire == DOWN && (global_frame_counter % 2) == 0) ||
          (global_frame_counter % 4) == 0)
        frame_main = (frame_main + 1) % 4;

      frame_ = frame_main;

      if (frame_ == 3)
        frame_ = 1;
    }
  }

  /* Duck! */
  if (input.down == DOWN && size == BIG && !duck 
      && physic.get_velocity_y() == 0 && on_ground())
    {
      duck = true;
      bbox.move(Vector(0, 32));
      bbox.set_height(32);
    }
  else if(input.down == UP && size == BIG && duck)
    {
      // try if we can really unduck
      bbox.move(Vector(0, -32));
      bbox.set_height(64);
      duck = false;
      // FIXME
#if 0
      // when unducking in air we need some space to do so
      if(on_ground() || !collision_object_map(bbox)) {
        duck = false;
      } else {
        // undo the ducking changes
        bbox.move(Vector(0, 32));
        bbox.set_height(32);
      }
#endif
    }
}

void
Player::grow(bool animate)
{
  if(size == BIG)
    return;
  
  size = BIG;
  bbox.set_height(64);
  bbox.move(Vector(0, -32));

  if(animate)
    growing_timer.start(GROWING_TIME);
}

void
Player::grabdistros()
{
}

void
Player::draw(DrawingContext& context)
{
  TuxBodyParts* tux_body;
          
  if (size == SMALL)
    tux_body = small_tux;
  else if (got_power == FIRE_POWER)
    tux_body = fire_tux;
  else if (got_power == ICE_POWER)
    tux_body = ice_tux;
  else
    tux_body = big_tux;

  int layer = LAYER_OBJECTS - 1;

  /* Set Tux sprite action */
  if (duck && size == BIG)
    {
    if(dir == LEFT)
      tux_body->set_action("duck-left");
    else // dir == RIGHT
      tux_body->set_action("duck-right");
    }
  else if (skidding_timer.started() && !skidding_timer.check())
    {
    if(dir == LEFT)
      tux_body->set_action("skid-left");
    else // dir == RIGHT
      tux_body->set_action("skid-right");
    }
  else if (kick_timer.started() && !kick_timer.check())
    {
    if(dir == LEFT)
      tux_body->set_action("kick-left");
    else // dir == RIGHT
      tux_body->set_action("kick-right");
    }
  else if (butt_jump && size == BIG)
    {
    if(dir == LEFT)
      tux_body->set_action("buttjump-left");
    else // dir == RIGHT
      tux_body->set_action("buttjump-right");
    }
  else if (physic.get_velocity_y() != 0)
    {
    if(dir == LEFT)
      tux_body->set_action("jump-left");
    else // dir == RIGHT
      tux_body->set_action("jump-right");
    }
  else
    {
    if (fabsf(physic.get_velocity_x()) < 1.0f) // standing
      {
      if(dir == LEFT)
        tux_body->set_action("stand-left");
      else // dir == RIGHT
        tux_body->set_action("stand-right");
      }
    else // moving
      {
      if(dir == LEFT)
        tux_body->set_action("walk-left");
      else // dir == RIGHT
        tux_body->set_action("walk-right");
      }
    }

  if(idle_timer.check())
    {
    if(size == BIG)
      {
      if(dir == LEFT)
        tux_body->head->set_action("idle-left");
      else // dir == RIGHT
        tux_body->head->set_action("idle-right");

      tux_body->head->start_animation(1);
      }

    }

  // Tux is holding something
  if ((holding_something && physic.get_velocity_y() == 0) ||
      (shooting_timer.get_timeleft() > 0 && !shooting_timer.check()))
    {
    if (duck)
      {
      if(dir == LEFT)
        tux_body->arms->set_action("duck+grab-left");
      else // dir == RIGHT
        tux_body->arms->set_action("duck+grab-right");
      }
    else
      {
      if(dir == LEFT)
        tux_body->arms->set_action("grab-left");
      else // dir == RIGHT
        tux_body->arms->set_action("grab-right");
      }
    }

  /* Draw Tux */
  if (dying == DYING_SQUISHED) {
    smalltux_gameover->draw(context, get_pos(), LAYER_FOREGROUNDTILES+1);
  } else if(growing_timer.get_timeleft() > 0) {
    if(size == SMALL)
      {
      if (dir == RIGHT)
        context.draw_surface(growingtux_right[GROWING_FRAMES-1 - 
                 int((growing_timer.get_timegone() *
                 GROWING_FRAMES) / GROWING_TIME)], get_pos(), layer);
      else
        context.draw_surface(growingtux_left[GROWING_FRAMES-1 - 
                int((growing_timer.get_timegone() *
                GROWING_FRAMES) / GROWING_TIME)], get_pos(), layer);
      }
    else
      {
      if (dir == RIGHT)
        context.draw_surface(growingtux_right[
            int((growing_timer.get_timegone() *
                GROWING_FRAMES) / GROWING_TIME)], get_pos(), layer);
      else
        context.draw_surface(growingtux_left[
            int((growing_timer.get_timegone() *
                             GROWING_FRAMES) / GROWING_TIME)],
            get_pos(), layer);
      }
    }
  else if (safe_timer.started() && global_frame_counter%2)
    ;  // don't draw Tux
  else
    tux_body->draw(context, get_pos(), layer);

  // Draw blinking star overlay
  if (invincible_timer.started() &&
     (invincible_timer.get_timeleft() > TUX_INVINCIBLE_TIME_WARNING
      || global_frame_counter % 3)
     && !dying)
  {
    if (size == SMALL || duck)
      smalltux_star->draw(context, get_pos(), LAYER_OBJECTS + 2);
    else
      bigtux_star->draw(context, get_pos(), LAYER_OBJECTS + 2);
  }
 
  if (debug_mode)
    context.draw_filled_rect(get_pos(),
        Vector(bbox.get_width(), bbox.get_height()),
        Color(75,75,75, 150), LAYER_OBJECTS+1);
}

HitResponse
Player::collision(GameObject& other, const CollisionHit& hit)
{
  if(dying) {
    return FORCE_MOVE;
  }

  if(other.get_flags() & FLAG_SOLID) {
    if(hit.normal.y < 0) { // landed on floor?
      if (physic.get_velocity_y() < 0)
        physic.set_velocity_y(0);
      on_ground_flag = true;
    } else if(hit.normal.y > 0) { // bumped against the roof
      physic.set_velocity_y(0);
    }
    
    if(hit.normal.x != 0) { // hit on the side?
      if(hit.normal.y > 0.6) // limits the slopes we can move up...
        physic.set_velocity_x(0);
    }

    return CONTINUE;
  }

  TriggerBase* trigger = dynamic_cast<TriggerBase*> (&other);
  if(trigger) {
    if(input.up == DOWN && input.old_up == UP)
      trigger->event(*this, TriggerBase::EVENT_ACTIVATE);
  }

  return FORCE_MOVE;
}

#if 0
void
Player::collision(void* p_c_object, int c_object)
{
  //BadGuy* pbad_c = NULL;
  //Trampoline* ptramp_c = NULL;
  //FlyingPlatform* pplatform_c = NULL;

  switch (c_object)
    {
    case CO_BADGUY:
      pbad_c = (BadGuy*) p_c_object;

     /* Hurt player if he touches a badguy */
      if (!pbad_c->dying && !dying &&
          !safe_timer.started() &&
          pbad_c->mode != BadGuy::HELD)
        {
          if (pbad_c->mode == BadGuy::FLAT && input.fire == DOWN
               && !holding_something)
            {
              holding_something = true;
              pbad_c->mode = BadGuy::HELD;
              pbad_c->base.y-=8;
            }
          else if (pbad_c->mode == BadGuy::FLAT)
            {
              // Don't get hurt if we're kicking a flat badguy!
            }
          else if (pbad_c->mode == BadGuy::KICK)
            {
              /* Hurt if you get hit by kicked laptop: */
              if (!invincible_timer.started())
                {
                  kill(SHRINK);
                }
              else
                pbad_c->kill_me(20);
            }
          else if (!pbad_c->frozen_timer.check() && (pbad_c->kind == BAD_MRBOMB
              || pbad_c->kind == BAD_JUMPY || pbad_c->kind == BAD_FISH
              || pbad_c->kind == BAD_SPIKY))
                pbad_c->kill_me(20);
          else
            {
              if (!invincible_timer.started())
                {
                  kill(SHRINK);
                }
              else
                {
                  pbad_c->kill_me(25);
                }
            }
          player_status.score_multiplier++;
        }
      break;

    case CO_TRAMPOLINE:
      ptramp_c = (Trampoline*) p_c_object;
      
      // Pick up trampoline
      if (ptramp_c->mode != Trampoline::M_HELD && input.fire == DOWN && !holding_something && on_ground())
      {
        holding_something = true;
        ptramp_c->mode = Trampoline::M_HELD;
        ptramp_c->base.y -= 8;
      }
      // Set down trampoline
      else if (ptramp_c->mode == Trampoline::M_HELD && input.fire != DOWN)
      {
        holding_something = false;
        ptramp_c->mode = Trampoline::M_NORMAL;
        ptramp_c->base.y += 8;
        ptramp_c->physic.set_velocity(physic.get_velocity_x(), physic.get_velocity_y());

        //if (dir == RIGHT)
        //  ptramp_c->base.x = base.x + base.width+1;
        //else /* LEFT */
        //  ptramp_c->base.x = base.x - base.width-1;
      }
/*
      // Don't let tux walk through trampoline
      else if (ptramp_c->mode != Trampoline::M_HELD && on_ground())
      {
        if (physic.get_velocity_x() > 0) // RIGHT
        {
          physic.set_velocity_x(0);
          base.x = ptramp_c->base.x - base.width;
        }
        else if (physic.get_velocity_x() < 0) // LEFT
        {
          physic.set_velocity_x(0);
          base.x = ptramp_c->base.x + ptramp_c->base.width;
        }
      }
*/
      break;
    case CO_FLYING_PLATFORM:
      pplatform_c = (FlyingPlatform*) p_c_object;
      
      base.y = pplatform_c->base.y - base.height;
      physic.set_velocity_x(pplatform_c->get_vel_x());
      
      physic.enable_gravity(false);
      can_jump = true;
      fall_mode = ON_GROUND;
      break;

    default:
      break;
    }

}
#endif

void
Player::make_invincible()
{
  SoundManager::get()->play_sound(IDToSound(SND_HERRING));
  invincible_timer.start(TUX_INVINCIBLE_TIME);
  Sector::current()->play_music(HERRING_MUSIC);               
}

/* Kill Player! */
void
Player::kill(HurtMode mode)
{
  if(dying)
    return;

  if(safe_timer.get_timeleft() > 0 || invincible_timer.get_timeleft() > 0)
    return;                          
  
  SoundManager::get()->play_sound(IDToSound(SND_HURT));

  physic.set_velocity_x(0);

  if (mode == SHRINK && size == BIG)
    {
      if (got_power != NONE_POWER)
        {
          safe_timer.start(TUX_SAFE_TIME);
          got_power = NONE_POWER;
        }
      else
        {
          growing_timer.start(GROWING_TIME);
          safe_timer.start(TUX_SAFE_TIME + GROWING_TIME);
          size = SMALL;
          bbox.set_height(32);
          duck = false;
        }
    }
  else
    {
      physic.enable_gravity(true);
      physic.set_acceleration(0, 0);
      physic.set_velocity(0, 700);
      --player_status.lives;
      dying = DYING_SQUISHED;
      dying_timer.start(3.0);
    }
}

/* Remove Tux's power ups */
void
Player::remove_powerups()
{
  got_power = NONE_POWER;
  size = SMALL;
  bbox.set_height(32);
}

void
Player::move(const Vector& vector)
{
  bbox.set_pos(vector);
}

void
Player::check_bounds(Camera* camera)
{
  /* Keep tux in bounds: */
  if (get_pos().x < 0)
    { // Lock Tux to the size of the level, so that he doesn't fall of
      // on the left side
      bbox.set_pos(Vector(0, get_pos().y));
    }

  /* Keep in-bounds, vertically: */
  if (get_pos().y > Sector::current()->solids->get_height() * 32)
    {
      kill(KILL);
      return;
    }

  bool adjust = false;
  // can happen if back scrolling is disabled
  if(get_pos().x < camera->get_translation().x) {
    bbox.set_pos(Vector(camera->get_translation().x, get_pos().y));
    adjust = true;
  }
  if(get_pos().x >= camera->get_translation().x + screen->w - bbox.get_width())
  {
    bbox.set_pos(Vector(
          camera->get_translation().x + screen->w - bbox.get_width(),
          get_pos().y));
    adjust = true;
  }

  if(adjust) {
    // FIXME
#if 0
    // squished now?
    if(collision_object_map(bbox)) {
      kill(KILL);
      return;
    }
#endif
  }
}

void
Player::bounce(BadGuy& badguy)
{
  //Make sure we stopped flapping
  flapping = false;
  falling_from_flap = false;
  
  if (input.jump)
    physic.set_velocity_y(520);
  else
    physic.set_velocity_y(200);
}

