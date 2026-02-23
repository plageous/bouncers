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

    void update()
    {
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
};

bn::fixed average_x(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers)
{
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

    return x_sum;
}

void add_bouncer(bn::vector<Bouncer, MAX_BOUNCERS> &bouncers)
{
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

    while (true)
    {
        if (bn::keypad::a_pressed()) {
            add_bouncer(bouncers);
        }
        if (bn::keypad::b_pressed()) {
            BN_LOG("Average x: ", average_x(bouncers));
        }

        // for each bouncer
        for (Bouncer &bouncer : bouncers) {
            bouncer.update();
        }

        bn::core::update();
    }
}