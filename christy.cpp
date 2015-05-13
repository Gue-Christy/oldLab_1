//my cpp file
#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <ctime>
#include <cmath>
#include <X11/Xlib.h>
#include <GL/glu.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "log.h"
extern "C" {
#include "struct.h"
#include "fonts.h"
#include "ppm.h"
}
#include "bullets.h"
#include "other.h"

extern void physics(Game *g)
{
        //Update player1 position
        g->player1.origin[0] = g->player1.pos[0];
        g->player1.origin[1] = g->player1.pos[1];

        g->player1.pos[0] += g->player1.vel[0];
        g->player1.pos[1] += g->player1.vel[1];
        //Check for collision with window edges
        //instantiate background change based on matrix
        //remove all zombies and objects and remake them
        if (g->player1.pos[0] < 0.0) {
                g->player1.pos[0] += (float)xres;
        }
        else if (g->player1.pos[0] > (float)xres) {
                g->player1.pos[0] -= (float)xres;
        }
        else if (g->player1.pos[1] < 0.0) {
                g->player1.pos[1] += (float)yres;
        }
        else if (g->player1.pos[1] > (float)yres) {
                g->player1.pos[1] -= (float)yres;
        }
        //
        //std::cout<<"Player X:" << g->player1.pos[0] <<" , Player Y:" << g->player1.pos[1] <<"\n";
        //
        //Update bullet positions
        updateBulletPos(g, g->bhead);
        if (g->player1.bulletType == 2 || g->player1.bulletType == 3) {
                updateBulletPos(g, g->chead);
                if(g->player1.bulletType == 3) {
                        updateBulletPos(g, g->dhead);
                }
        }
 updateMulti(g);
        //
        //Update asteroid positions
        Zombie *a = g->ahead;
        while (a) {
                //Try nesting everything in an if/else with a randomized bool
                //to determine if zombie is wandering or running at player?
                zMove(g, a);
                //zomb_zomb_collision(a);
                a->pos[0] += a->vel[0];
                a->pos[1] += a->vel[1];
                //Check for collision with window edges
                if (a->pos[0] < -100.0) {
                        a->pos[0] += (float)xres+200;
                }
                else if (a->pos[0] > (float)xres+100) {
                        a->pos[0] -= (float)xres+200;
                }
                else if (a->pos[1] < -100.0) {
                        a->pos[1] += (float)yres+200;
                }
                else if (a->pos[1] > (float)yres+100) {
                        a->pos[1] -= (float)yres+200;
                }
                a->angle += a->rotate;
                a = a->next;
        }
        //Zombie collision with bullets?
        //If collision detected:
        //     1. delete the bullet
        //     2. break the asteroid into pieces
        //        if asteroid small, delete it
        bul_zomb_collision(g, g->bhead);
        if (g->player1.bulletType == 2 || g->player1.bulletType == 3) {
                bul_zomb_collision(g, g->chead);
                if (g->player1.bulletType == 3) {
                        bul_zomb_collision(g, g->dhead);
                }
        }
 //Player collision with zombies
        player_zomb_collision(g);
        if(g->gameover) {
                std::cout<<"returning again\n";
                return;
        }
        //---------------------------------------------------
        //check keys pressed now
        //NOTE:: ANGLE CHECKED COUNTER CLOCKWISE

        last_Position_S = g->player1.angle;
        //BREAK if attempting to move opposite directions at same time
        if (((keys[XK_Left] || keys[XK_a]) && (keys[XK_Right] || keys[XK_d])) ||
                        ((keys[XK_Up]   || keys[XK_w]) && (keys[XK_Down]  || keys[XK_s]))) {
                //convert player1 angle to radians
                //convert angle to a vector
                g->player1.vel[0] = 0;
                g->player1.vel[1] = 0;
                g->player1.angle = last_Position_S;
                if (g->player1.angle >= 360.0f)
                        g->player1.angle -= 360.0f;
        }
else if ((keys[XK_Up] || keys[XK_w]) && (keys[XK_Left] || keys[XK_a])) {
                normalize(g->player1.vel);
                g->player1.vel[0] = -4;
                g->player1.vel[1] = 4;
        }
        else if ((keys[XK_Down] || keys[XK_s]) && (keys[XK_Left] || keys[XK_a])) {
                normalize(g->player1.vel);
                g->player1.vel[0] = -4;
                g->player1.vel[1] = -4;
        }
        else if ((keys[XK_Down] || keys[XK_s]) && (keys[XK_Right] || keys[XK_d])) {
                normalize(g->player1.vel);
                g->player1.vel[0] = 4;
                g->player1.vel[1] = -4;

        }
        else if ((keys[XK_Up] || keys[XK_w]) && (keys[XK_Right] || keys[XK_d])) {
                normalize(g->player1.vel);
                g->player1.vel[0] = 4;
                g->player1.vel[1] = 4;

        }
        else if (keys[XK_Left] || keys[XK_a]) {
                normalize(g->player1.vel);
                g->player1.vel[0] = -8;

        }
        else if (keys[XK_Right] || keys[XK_d]) {
                normalize(g->player1.vel);
                g->player1.vel[0] = 8;

        }
        else if (keys[XK_Up] || keys[XK_w]) {
                normalize(g->player1.vel);
                g->player1.vel[1] = 8;

        }
else if (keys[XK_Down] || keys[XK_s]) {
                normalize(g->player1.vel);
                g->player1.vel[1] = -8;

        }
        else {
                //convert player1 angle to radians
                //convert angle to a vector
                g->player1.vel[0] = 0;
                g->player1.vel[1] = 0;
                g->player1.angle = last_Position_S;
                if (g->player1.angle >= 360.0f)
                        g->player1.angle -= 360.0f;

        }
bresenham_Ang(g);
        if (keys[XK_i]) {
                g->player1.invuln++;
                if (g->player1.invuln == 2)
                        g->player1.invuln = 0;
                keys[XK_i] = 0;
        }

        if (keys[XK_space]) {
                fire_weapon(g);
        }
        if (g->player1.is_firing) {
                fire_weapon(g);
        }

        if (keys[XK_1]) {
                g->player1.bulletType = 1;

        } else if (keys[XK_2]) {
                g->player1.bulletType = 2;

        } else if (keys[XK_3]) {
                g->player1.bulletType = 3;
        }
}

