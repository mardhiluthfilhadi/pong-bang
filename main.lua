local ffi  = require "ffi"
local test = ffi.load "./libs/libtest.so"

ffi.cdef [[

typedef struct {
    float x,y;
} Vec2;

typedef struct {
    float x,y,w,h;
} Vec4;

typedef struct {
    Vec4 paddle_l;
    Vec4 paddle_r;
    Vec2 ball;
    float ball_radius;
    int score_l, score_r;

    float elapsed;
} To_Draw;

Vec2 restart(int keep_score);
int update(float dt, int dir, int res);
To_Draw draw(void);

]]

local font_score
local font_debug
local ball_sfx = {}

local BG_CANVAS
local restart = 0
local screen = ffi.new("Vec2", 800, 900)

function love.load()
    font_score=love.graphics.newFont("assets/VictorMono-Medium.otf", 46)
    font_debug=love.graphics.newFont("assets/VictorMono-Medium.otf", 22)
    
    for i=1, 5 do
        ball_sfx[i]=love.audio.newSource("assets/ball.ogg", "static")
    end

    local s = test.restart(0)
    screen.x, screen.y = s.x, s.y

    love.window.setMode(screen.x, screen.y)
    BG_CANVAS = love.graphics.newCanvas(screen.x, screen.y)

    love.graphics.setCanvas(BG_CANVAS)
        love.graphics.clear(0x22/0xff, 0xee/0xff, 0x16/0xff)
        love.graphics.setColor(0x1f/0xff, 0xee/0xff, 0x2d/0xff)
        love.graphics.rectangle(
            "fill",
            screen.x/2, 0, screen.x/2, screen.y
        )
        
        love.graphics.setColor(1,1,1)
        love.graphics.rectangle(
            "fill",
            screen.x/2-1, 0,
            2, screen.y
        )

        love.graphics.circle(
            "line",
            screen.x/2, screen.y/2,
            screen.y/3
        )
    love.graphics.setCanvas()
end

function love.update(dt)
    local dir = (love.keyboard.isDown("up") and 1) or
        (love.keyboard.isDown("down") and 2) or 0
    if dir == 0 and love.mouse.isDown(1) then
        local x,y = love.mouse.getPosition()
        if x < screen.x*0.5 then
            if y < screen.y*0.5 then
                dir = 1
            elseif y > screen.y*0.5 then
                dir = 2
            end
        end
    end

    local play_ball_sfx = test.update(dt, dir, restart)
    if play_ball_sfx==1 then
        for i=1,5 do
            if not ball_sfx[i]:isPlaying() then
                love.audio.play(ball_sfx[i])
                break
            end
        end
    end

    if restart ~= 0 then restart = 0 end
end

function love.draw()
    local to_draw = test.draw()
    local pl = to_draw.paddle_l
    local pr = to_draw.paddle_r
    local bl = to_draw.ball
    local sl,sr = to_draw.score_l, to_draw.score_r

    if BG_CANVAS then
        love.graphics.draw(BG_CANVAS)
    end

    love.graphics.setColor(1,1,1)
    
    love.graphics.rectangle(
        "fill", pl.x, pl.y, pl.w, pl.h, 12
    )
    
    love.graphics.rectangle(
        "fill", pr.x, pr.y, pr.w, pr.h, 12
    )

    love.graphics.circle(
        "fill", bl.x, bl.y, to_draw.ball_radius
    )

    if font_score then
        if font_score then
            love.graphics.setFont(font_score)
        end

        local sls = tostring(sl)
        local srs = tostring(sr)
        local slw = font_score:getWidth(sls)
        local srw = font_score:getWidth(srs)
        love.graphics.print(sls, screen.x/2 - slw*2, 10)
        love.graphics.print(srs, screen.x/2 + srw, 10)
    end

    if font_debug then
        if font_debug then
            love.graphics.setFont(font_debug)
        end
        
        love.graphics.print("FPS: "..love.timer.getFPS(), 10, 10)
        love.graphics.print("Elapsed: "..to_draw.elapsed, 10, 40)
    end
end

function love.keypressed(key)
    if key == "escape" then
        love.event.quit()
    elseif key == "space" then
        restart = 1
    end
end
