#include "game.h"
using namespace std;

bool initialize(SDL_Window* &gWindow, SDL_Renderer* &gRenderer)
{
    //Initialization flag
	bool success = true;

    if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0 || TTF_Init() == -1)
    {
        cout << "SDL could not initialize! SDL Error: " << SDL_GetError() << ", " << TTF_GetError() << endl;
        success = false;
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
        if( gWindow == NULL )
        {
            cout << "Window could not be created! SDL Error: " << SDL_GetError() << endl;
            success = false;
        }
        else
        {
            //Create renderer for window
            Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
            gRenderer = SDL_CreateRenderer( gWindow, -1, render_flags );
            if( gRenderer == NULL )
            {
                cout << "Renderer could not be created! SDL Error: " << SDL_GetError() << endl;
                success = false;
            }
        }
    }

    return success;
}

void close(SDL_Window* &gWindow, SDL_Renderer* &gRenderer)
{
    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    //Quit SDL
    TTF_Quit();
    SDL_Quit();
}

void Loading_and_Setting(SDL_Renderer* gRenderer, Texture &backGround, Text &messages,
         bird &BIRD, LowerPipe &L1, LowerPipe &L2, LowerPipe &L3, UpperPipe &U1, UpperPipe &U2, UpperPipe &U3,
         Audio &score_sound, Audio &flap_sound, Audio &hit_sound)
{
    //background
    backGround.loadFromFile("img/background.bmp", gRenderer);
    backGround.W = SCREEN_WIDTH;
    backGround.H = SCREEN_HEIGHT;
    backGround.update_sprite();

    //text
    messages.load("Press any key to play", 14, gRenderer );

    //set bird
    BIRD.loadFromFile("img/bird_animation.bmp", gRenderer);
    BIRD.setClips();
    BIRD.W = BIRD_W;
    BIRD.H = BIRD_H;

    //set pipes
    L1.loadFromFile("img/LowerPipe.bmp", gRenderer);
    L1.W = PIPE_WIDTH;
    L1.H = PIPE_HEIGHT;
    L1.x = SCREEN_WIDTH;
    L1.y = PIPE_VERTICAL_DIST + 100 + rand()%300 ;

    U1.loadFromFile("img/UpperPipe.bmp", gRenderer);
    U1.W = PIPE_WIDTH;
    U1.H = PIPE_HEIGHT;
    U1.update_pos(L1);

    L2.loadFromFile("img/LowerPipe.bmp", gRenderer);
    L2.W = PIPE_WIDTH;
    L2.H = PIPE_HEIGHT;
    L2.x = L1.x + PIPE_HORIZONTAL_DIST;
    L2.y = PIPE_VERTICAL_DIST + 100 + rand()%300 ;

    U2.loadFromFile("img/UpperPipe.bmp", gRenderer);
    U2.W = PIPE_WIDTH;
    U2.H = PIPE_HEIGHT;
    U2.update_pos(L2);

    L3.loadFromFile("img/LowerPipe.bmp", gRenderer);
    L3.W = PIPE_WIDTH;
    L3.H = PIPE_HEIGHT;
    L3.x = L2.x + PIPE_HORIZONTAL_DIST;
    L3.y = PIPE_VERTICAL_DIST + 100 + rand()%300 ;

    U3.loadFromFile("img/UpperPipe.bmp", gRenderer);
    U3.W = PIPE_WIDTH;
    U3.H = PIPE_HEIGHT;
    U3.update_pos(L3);

    //Load sound effects
    score_sound.load("sound/sfx_score.wav");
    flap_sound.load("sound/sfx_flap.wav");
    hit_sound.load("sound/sfx_hit.wav");
}

bool play(SDL_Window* &gWindow, SDL_Renderer* &gRenderer)
{
    srand(time(0));
    bool start_Game = false;
    int score = 0;
    int frame_Rendered = 0; //numbers of frame rendered

    //Objects declaration
    Texture backGround;
    Text messages;
    bird BIRD;
    LowerPipe L1, L2, L3;
    UpperPipe U1, U2, U3;
    Audio score_sound, flap_sound, hit_sound;

    //Load images and set objects' initial properties
    Loading_and_Setting(gRenderer, backGround, messages, BIRD, L1, L2, L3, U1, U2, U3, score_sound, flap_sound, hit_sound);

    //Event handler
    SDL_Event e;

    //Initial Animation loop
    BIRD.update_pos();
    while( !start_Game )
    {
        //Handle events on queue
        while( SDL_PollEvent(&e) != 0 )
        {
            if (e.type == SDL_QUIT) return false;
            if (e.type == SDL_KEYDOWN) {
                start_Game = true;
                BIRD.flap(); //Bird's initial flap when the game begins
                messages.load( to_string(score), 36, gRenderer );
            }
        }
        SDL_RenderClear( gRenderer );
        SDL_RenderCopy( gRenderer, backGround.texture, NULL, &backGround.sprite );
        SDL_RenderCopy( gRenderer, messages.texture, NULL, &messages.clipBoard);
        SDL_RenderCopyEx( gRenderer, BIRD.texture, &BIRD.clip[1], &BIRD.sprite, 0, NULL, SDL_FLIP_NONE);
        SDL_RenderPresent( gRenderer );
        SDL_Delay(1000/FPS);
    }

    //Main Animation loop
    while( 1 )
    {
        //Handle events on queue
        while( SDL_PollEvent(&e) != 0 )
        {
            if (e.type == SDL_QUIT) return false;
            if (e.type == SDL_KEYDOWN) {
                BIRD.flap();
                flap_sound.play();
            }
        }

        //Update objects' positions
        BIRD.update_pos();
        L1.update_pos(L3,score);
        U1.update_pos(L1);
        L2.update_pos(L1,score);
        U2.update_pos(L2);
        L3.update_pos(L2,score);
        U3.update_pos(L3);

        //Clear screen
        SDL_RenderClear( gRenderer );

        //Render
        SDL_RenderCopy( gRenderer, backGround.texture, NULL, &backGround.sprite );
        SDL_RenderCopy( gRenderer, L1.texture, NULL, &L1.sprite );
        SDL_RenderCopy( gRenderer, U1.texture, NULL, &U1.sprite );
        SDL_RenderCopy( gRenderer, L2.texture, NULL, &L2.sprite );
        SDL_RenderCopy( gRenderer, U2.texture, NULL, &U2.sprite );
        SDL_RenderCopy( gRenderer, L3.texture, NULL, &L3.sprite );
        SDL_RenderCopy( gRenderer, U3.texture, NULL, &U3.sprite );
        SDL_RenderCopy( gRenderer, messages.texture, NULL, &messages.clipBoard);
        SDL_RenderCopyEx( gRenderer, BIRD.texture, &BIRD.clip[ (frame_Rendered/2)%3 ], &BIRD.sprite, BIRD.getAngle(), NULL, SDL_FLIP_NONE);

        //Update screen
        SDL_RenderPresent( gRenderer );

        //Check Collision
        if ( SDL_HasIntersection(&BIRD.sprite, &L1.sprite) ) { hit_sound.play(); break; }
        if ( SDL_HasIntersection(&BIRD.sprite, &U1.sprite) ) { hit_sound.play(); break; }
        if ( SDL_HasIntersection(&BIRD.sprite, &L2.sprite) ) { hit_sound.play(); break; }
        if ( SDL_HasIntersection(&BIRD.sprite, &U2.sprite) ) { hit_sound.play(); break; }
        if ( SDL_HasIntersection(&BIRD.sprite, &L3.sprite) ) { hit_sound.play(); break; }
        if ( SDL_HasIntersection(&BIRD.sprite, &U3.sprite) ) { hit_sound.play(); break; }
        if ( BIRD.sprite.y < 0) { hit_sound.play(); break; }
        if ( BIRD.is_OnTheGround() ) { hit_sound.play(); break; }

        //Count score & generate sound
        frame_Rendered++;
        if ( frame_Rendered > 75 && frame_Rendered % 75 == 0 )
        {
            score++;
            score_sound.play();
            messages.load( to_string(score), 36, gRenderer );
        }

        //Delay 1/FPS second
        SDL_Delay(1000/FPS);
    }

    //The bird falls
    while ( !BIRD.is_OnTheGround() )
    {
        BIRD.update_pos();
        SDL_RenderClear( gRenderer );
        SDL_RenderCopy( gRenderer, backGround.texture, NULL, &backGround.sprite );
        SDL_RenderCopy( gRenderer, L1.texture, NULL, &L1.sprite );
        SDL_RenderCopy( gRenderer, U1.texture, NULL, &U1.sprite );
        SDL_RenderCopy( gRenderer, L2.texture, NULL, &L2.sprite );
        SDL_RenderCopy( gRenderer, U2.texture, NULL, &U2.sprite );
        SDL_RenderCopy( gRenderer, L3.texture, NULL, &L3.sprite );
        SDL_RenderCopy( gRenderer, U3.texture, NULL, &U3.sprite );
        SDL_RenderCopyEx( gRenderer, BIRD.texture, &BIRD.clip[1], &BIRD.sprite, 90, NULL, SDL_FLIP_NONE);
        SDL_RenderPresent( gRenderer );
        SDL_Delay(1000/FPS);
    }
    SDL_Delay(300);

    if ( end_Game(gRenderer, e, score, backGround, messages, BIRD, L1, L2, L3, U1, U2, U3, score_sound, flap_sound, hit_sound) ) return false;
    else return true;
}

bool end_Game(SDL_Renderer* gRenderer, SDL_Event &e, int &score, Texture &backGround, Text &messages,
         bird &BIRD, LowerPipe &L1, LowerPipe &L2, LowerPipe &L3, UpperPipe &U1, UpperPipe &U2, UpperPipe &U3,
         Audio &score_sound, Audio &flap_sound, Audio &hit_sound)
{
    //Gameover banner
    Texture gameover;
    gameover.loadFromFile("img/gameover.bmp", gRenderer);
    gameover.W = 300;
    gameover.H = 70;
    gameover.x = (SCREEN_WIDTH - gameover.W) / 2;
    gameover.y = 50;
    gameover.update_sprite();
    SDL_RenderCopy( gRenderer, gameover.texture, NULL, &gameover.sprite);

    //Display clipboard
    Texture clipBoard;
    clipBoard.loadFromFile("img/clipboard.bmp", gRenderer);
    clipBoard.W *= 4;
    clipBoard.H *= 2;
    clipBoard.x = (SCREEN_WIDTH - clipBoard.W) / 2;
    clipBoard.y = 130;
    clipBoard.update_sprite();
    SDL_RenderCopy(gRenderer, clipBoard.texture, NULL, &clipBoard.sprite);

    //Display final score
    messages.load( "Your score: " + to_string(score) , 26, gRenderer);
    SDL_RenderCopy( gRenderer, messages.texture, NULL, &messages.clipBoard);

    //Display message
    messages.load( "Press any key to play again" , 14, gRenderer);
    messages.clipBoard.y = 200;
    SDL_RenderCopy( gRenderer, messages.texture, NULL, &messages.clipBoard);
    SDL_RenderPresent( gRenderer );

    //Deallocate objects
    backGround.free();
    messages.free();
    BIRD.free();
    L1.free(); L2.free(); L3.free(); U1.free(); U2.free(); U3.free();
    score_sound.~Audio(); flap_sound.~Audio(); hit_sound.~Audio();

    //Check if player want to play again
    while ( 1 ) {
        while( SDL_PollEvent(&e) != 0 )
            {
                if (e.type == SDL_QUIT) {
                    return true;
                }
                if (e.type == SDL_KEYDOWN) {
                    return false;
                }
            }
        SDL_Delay(100);
    }
}