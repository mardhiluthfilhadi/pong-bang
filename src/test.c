#include <math.h>
#include <stdlib.h>

typedef struct {
    float x,y;
} Vec2;

Vec2 vec2_normalize(Vec2 v) {
    Vec2 res = v;

    float len = sqrtf(v.x * v.x + v.y * v.y);
    if (len > 0.00010f) {
        res.x /= len;
        res.y /= len;
    }

    return res;
}

Vec2 vec2_reflect(Vec2 v, Vec2 p) {
    Vec2 n = vec2_normalize(p);
    float dot = v.x * n.x + v.y * n.y;

    Vec2 res;
    res.x = v.x - (2 * dot * n.x);
    res.y = v.y - (2 * dot * n.y);

    return res;
}

Vec2 vec2_from_angle(float a, float len) {
    Vec2 res;
    res.x = cosf(a) * len;
    res.y = sinf(a) * len;

    return res;
}

float vec2_look_at(Vec2 a, Vec2 b) {
    return atan2f(b.y - a.y, b.x - a.x);
}

typedef struct {
    float x,y,w,h;
} Vec4;

#define FALSE 0
#define TRUE  1
int is_circle_rect_collide(Vec2 c, float r, Vec4 rect) {
    float x = fmax(rect.x, fmin(c.x, rect.x + rect.w));
    float y = fmax(rect.y, fmin(c.y, rect.y + rect.h));

    float dx = c.x - x;
    float dy = c.y - y;
    return (dx*dx + dy*dy) < r*r;
}

typedef struct {
    Vec4 paddle_l;
    Vec4 paddle_r;
    Vec2 ball;
    float ball_radius;
    int score_l, score_r;
    float elapsed;
} To_Draw;

#define SCREEN_WIDTH  800.0f
#define SCREEN_HEIGHT 600.0f
#define SCREEN_CX SCREEN_WIDTH *0.5f
#define SCREEN_CY SCREEN_HEIGHT*0.5f

#define BALL_RADIUS 10.0f
Vec2 ball;
Vec2 ball_dir;
float ball_speed = 500.0f;

#define PADDLE_L 20.0f
#define PADDLE_R SCREEN_WIDTH - 40.0f
#define PADDLE_WIDTH  20.0f
#define PADDLE_HEIGHT 120.0f
float paddle_l;
float paddle_r;
float paddle_speed = 400.0f;

int paddle_l_dir = 0;
int paddle_r_dir = 0;

int started   = FALSE;
int paused    = FALSE;
float elapsed = 7.856f;

#define LEFT_PAD  1
#define RIGHT_PAD 2
int score_l, score_r;
int last_scored = LEFT_PAD;

Vec2 restart(int full_wipe) {
    started = FALSE;
    elapsed = 7.856f;

    if (!full_wipe) {
        score_l = 0;
        score_r = 0;
        paddle_l = SCREEN_CY;
        paddle_r = SCREEN_CY;
    }

    ball = (Vec2) {SCREEN_CX, SCREEN_CY};
    ball_dir = (Vec2) {0.0f, 1.0f};
    ball_speed = 500.0f;

    paddle_r_dir = 0;

    return (Vec2) {SCREEN_WIDTH, SCREEN_HEIGHT};
}

void update_not_started(float dt, int dir) {
    if (last_scored == LEFT_PAD && dir) {
        started = TRUE;
        
        Vec2 padd_c = (Vec2) {PADDLE_L, paddle_l};
        float angle = vec2_look_at(ball, padd_c);
        ball_dir = vec2_from_angle(angle, 1.0f);
        return;
    }

    if (last_scored == RIGHT_PAD && (rand()%10) > 7) {
        started = TRUE;
        
        Vec2 padd_c = (Vec2) {PADDLE_R, paddle_r};
        float angle = vec2_look_at(ball, padd_c);
        ball_dir = vec2_from_angle(angle, 1.0f);
        return;
    }
    
    elapsed += dt*3;
    float delta = cosf(elapsed) * (SCREEN_CY - BALL_RADIUS);
    ball.y = SCREEN_CY + delta;
}

void resolve_ball_to_paddle_collision(
    Vec2 *np, Vec4 pad, int left
) {
    if (ball.x >= pad.x && ball.x <= pad.x+pad.w) { 
        if (ball.y+BALL_RADIUS < pad.y + pad.h*0.5f) {
            np->y = pad.y - BALL_RADIUS;
            ball_dir = vec2_reflect(ball_dir, (Vec2){
                0.0f,-1.0f
            });
        } else if (ball.y-BALL_RADIUS > pad.y + pad.h*0.5f){
            np->y = pad.y + pad.w + BALL_RADIUS;
            ball_dir = vec2_reflect(ball_dir, (Vec2){
                0.0f, 1.0f
            });
        }
    }

    int do_bounce = (left) ?
        ball.x-BALL_RADIUS >= pad.x+pad.w :
        ball.x+BALL_RADIUS <= pad.x;
        
    if (do_bounce) {
        np->x = (left) ? 
            pad.x + pad.w + BALL_RADIUS :
            pad.x - BALL_RADIUS;

        ball_dir = vec2_reflect(ball_dir, (Vec2){
            1.0f, 0.0f
        });
    }
}

void update_cpu_paddle(float dt) {
    if (ball_dir.x <= 0) return;

    if (ball.y < paddle_r-PADDLE_HEIGHT*0.5f) {
        paddle_r_dir = -1;
    } else if (ball.y > paddle_r+PADDLE_HEIGHT*0.5f) {
        paddle_r_dir =  1;
    } else {
        paddle_r_dir = 0;
    }

    float dist = fabsf(paddle_r - ball.y);
    float speed = dist*2 + ball_speed * 0.2;
    speed *= (float) paddle_r_dir;

    paddle_r += speed * dt;
    
    paddle_r = (paddle_r <= PADDLE_HEIGHT*0.5f) ? 
        PADDLE_HEIGHT*0.5f : paddle_r;

    paddle_r = (paddle_r>=SCREEN_HEIGHT-PADDLE_HEIGHT*.5f)? 
        SCREEN_HEIGHT-PADDLE_HEIGHT*0.5f : paddle_r;
}

int update_ball(float dt) {
    Vec2 new_pos;
    new_pos.x = ball.x + ball_dir.x * ball_speed * dt;
    new_pos.y = ball.y + ball_dir.y * ball_speed * dt;

    Vec4 rect = (Vec4){
        PADDLE_L, paddle_l - PADDLE_HEIGHT*0.5f,
        PADDLE_WIDTH, PADDLE_HEIGHT
    };

    int do_ball_sfx = FALSE;
    
    if (is_circle_rect_collide(new_pos, BALL_RADIUS, rect)) {
        if (
            ball_dir.y > 0 && paddle_l_dir > 0 ||
            ball_dir.y < 0 && paddle_l_dir < 0
        ) {
            ball_speed *= 1.3f;
        } 
        resolve_ball_to_paddle_collision(&new_pos, rect, 1);
        do_ball_sfx = TRUE;
    }
    
    rect.x = PADDLE_R;
    rect.y = paddle_r - PADDLE_HEIGHT*0.5f;
    
    if (is_circle_rect_collide(new_pos, BALL_RADIUS, rect)) {
        resolve_ball_to_paddle_collision(&new_pos, rect, 0);
        do_ball_sfx = TRUE;
    }
    
    if (new_pos.y <=BALL_RADIUS) {
        new_pos.y = BALL_RADIUS;
        ball_dir = vec2_reflect(ball_dir, (Vec2){
            0.0f, 1.0f
        });

        do_ball_sfx = TRUE;
    } else if (new_pos.y >= SCREEN_HEIGHT-BALL_RADIUS) {
        new_pos.y = SCREEN_HEIGHT-BALL_RADIUS;
        ball_dir = vec2_reflect(ball_dir, (Vec2){
            0.0f,-1.0f
        });

        do_ball_sfx = TRUE;
    }

    ball_dir = vec2_normalize(ball_dir);
    ball = new_pos;

    return do_ball_sfx;
}


int update(float dt, int dir, int res) {
    if (res) restart(FALSE);
    if (paused) return FALSE;

    if (!started) {
        update_not_started(dt, dir);
        return FALSE;
    }
    
    paddle_l_dir = 0;
    switch (dir) {
        case 1: { paddle_l_dir = -1; break; }
        case 2: { paddle_l_dir =  1; break; }
        default: break;
    }

    float speed = paddle_speed + ball_speed*0.2;
    paddle_l += speed * paddle_l_dir * dt;
    update_cpu_paddle(dt);
    
    float bound_up   = PADDLE_HEIGHT*0.5f;
    float bound_down = SCREEN_HEIGHT - PADDLE_HEIGHT*0.5f;

    paddle_l = (paddle_l <= bound_up) 
        ? bound_up : paddle_l;

    paddle_l = (paddle_l >= bound_down) 
        ? bound_down : paddle_l;

    int do_ball_sfx = update_ball(dt);

    if (ball.x >= SCREEN_WIDTH+BALL_RADIUS) {
        last_scored = LEFT_PAD;
        score_l += 1;
        restart(TRUE);
    } else if (ball.x <= -BALL_RADIUS) {
        last_scored = RIGHT_PAD;
        score_r += 1;
        restart(TRUE);
    }

    return do_ball_sfx;
}

To_Draw draw() {
    To_Draw result;
    
    result.paddle_l = (Vec4){
        PADDLE_L, paddle_l - PADDLE_HEIGHT*0.5f,
        PADDLE_WIDTH, PADDLE_HEIGHT
    };
    
    result.paddle_r = (Vec4){
        PADDLE_R, paddle_r - PADDLE_HEIGHT*0.5f,
        PADDLE_WIDTH, PADDLE_HEIGHT
    };
    
    result.ball_radius = BALL_RADIUS;
    result.ball = ball;

    result.score_l = score_l;
    result.score_r = score_r;
    result.elapsed = elapsed;

    return result;
}
