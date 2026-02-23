#include <bn_core.h>
#include <bn_backdrop.h>
#include <bn_keypad.h>
#include <bn_sprite_ptr.h>
#include <bn_display.h>
#include <bn_random.h>
#include <bn_vector.h>
#include <bn_log.h>

#include "bn_sprite_items_dot.h"

// Set max/min x position to be the edges of the display
static constexpr int HALF_SCREEN_WIDTH = bn::display::width() / 2;
static constexpr int HALF_SCREEN_HEIGHT = bn::display::height() / 2;
static constexpr bn::fixed MIN_X = -HALF_SCREEN_WIDTH;
static constexpr bn::fixed MAX_X = HALF_SCREEN_WIDTH;
static constexpr bn::fixed MIN_Y = -HALF_SCREEN_HEIGHT;
static constexpr bn::fixed MAX_Y = HALF_SCREEN_HEIGHT;

// Maximum number of bouncers on screen at once
static constexpr int MAX_BOUNCERS = 20;
static bn::random rng = bn::random();

class Bouncer
{
public:
    bn::sprite_ptr sprite = bn::sprite_items::dot.create_sprite();
    bn::fixed x_speed = rng.get_fixed(-5, 5);
    bn::fixed y_speed = rng.get_fixed(-5, 5);

    // default constructor
    void update() {
        bn::fixed x = sprite.x();
        bn::fixed y = sprite.y();

        // Update x and y position by adding speed
        x += x_speed;
        y += y_speed;

        // If we've gone off the screen on the right
        if (x > MAX_X) {
            // Snap back to screen and reverse direction
            x = MAX_X;
            x_speed *= -1;
        }
        // If we've gone off the screen on the left
        if (x < MIN_X) {
            // Snap back to screen and reverse direction
            x = MIN_X;
            x_speed *= -1;
        }
        // If we've gone off screen on the bottom
        if (y > MAX_Y) {
            // Snap back and reverse
            y = MAX_Y;
            y_speed *= -1;
        }
        // If we've gone off screen on the bottom
        if (y < MIN_Y) {
            // Snap back and reverse
            y = MIN_Y;
            y_speed *= -1;
        }

        sprite.set_x(x);
        sprite.set_y(y);
    }

    // override for average dot
    void update(bn::fixed x_average, bn::fixed y_average) {   
        sprite.set_x(x_average);
        sprite.set_y(y_average);
    }
};

bn::fixed average_x_speed(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers) {
    // Add all x positions together
    bn::fixed x_sum = 0;
    for (Bouncer bouncer : bouncers)
    {
        x_sum += bouncer.x_speed;
    }

    bn::fixed x_average = x_sum;

    // Only divide if we have 1 or more
    // Prevents division by 0
    if (bouncers.size() > 0)
    {
        x_average /= bouncers.size();
    }

    return x_average;
}

// returns average y speed of all bouncers
bn::fixed average_y_speed(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers) {
    // Add all x positions together
    bn::fixed y_sum = 0;
    for (Bouncer bouncer : bouncers)
    {
        y_sum += bouncer.y_speed;
    }

    bn::fixed y_average = y_sum;

    // Only divide if we have 1 or more
    // Prevents division by 0
    if (bouncers.size() > 0)
    {
        y_average /= bouncers.size();
    }

    return y_average;
}

bn::fixed average_x_pos(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers) {
    // Add all x positions together
    bn::fixed x_sum = 0;
    for (int i = 1; i < bouncers.size(); i++)
    {
        x_sum += bouncers[i].sprite.x();
    }

    bn::fixed x_average = x_sum;

    // Prevents the average dot from being used in
    // the average. Prevents division by 0.
    if (bouncers.size() > 1) {
        x_average /= (bouncers.size() - 1);
    }
    return x_average;
}

bn::fixed average_y_pos(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers) {
    // Add all x positions together
    bn::fixed y_sum = 0;
    for (int i = 1; i < bouncers.size(); i++)
    {
        y_sum += bouncers[i].sprite.y();
    }

    bn::fixed y_average = y_sum;

    // Prevents the average dot from being used in
    // the average. Prevents division by 0.
    if (bouncers.size() > 1) {
        y_average /= (bouncers.size() - 1);
    }
    return y_average;
}

void add_bouncer(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers) {
    rng.update();
    // Only add if we're below the maximum
    if (bouncers.size() < bouncers.max_size())
    {
        bouncers.push_back(Bouncer());
    }
}

int main() {
    bn::core::init();
    bn::vector<Bouncer, MAX_BOUNCERS> bouncers = {};

    // add an initial bouncer for the average bouncer
    add_bouncer(bouncers);

    while (true)
    {
        if (bn::keypad::a_pressed()) {
            add_bouncer(bouncers);
        }
        if (bn::keypad::b_pressed()) {
            BN_LOG("Average x: ", average_x_speed(bouncers));
        }

        // for each bouncer
        for (Bouncer &bouncer : bouncers) {
            bouncer.update();
        }

        // update the average bouncer
        bouncers[0].update(average_x_pos(bouncers), average_y_pos(bouncers));

        bn::core::update();
    }
}