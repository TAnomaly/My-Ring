#include <iostream>
#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>

using namespace std;
using namespace cv;

class SDLWebcam
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    VideoCapture cap;
    int width, height;

public:
    SDLWebcam(const string &url)
    {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            throw runtime_error("SDL init failed: " + string(SDL_GetError()));
        }

        // Initialize OpenCV
        cap.open(url);
        if (!cap.isOpened())
        {
            throw runtime_error("Cannot open video: " + url);
        }

        width = (int)cap.get(CAP_PROP_FRAME_WIDTH);
        height = (int)cap.get(CAP_PROP_FRAME_HEIGHT);

        cout << "Stream: " << width << "x" << height << endl;

        // Create window
        window = SDL_CreateWindow("DroidCam Live - SDL2",
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

        if (!window)
        {
            throw runtime_error("Window creation failed: " + string(SDL_GetError()));
        }

        // Create renderer
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
        {
            throw runtime_error("Renderer creation failed: " + string(SDL_GetError()));
        }

        // Create texture for video frames
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24,
                                    SDL_TEXTUREACCESS_STREAMING, width, height);

        if (!texture)
        {
            throw runtime_error("Texture creation failed: " + string(SDL_GetError()));
        }

        cout << "SDL2 window created successfully!" << endl;
    }

    ~SDLWebcam()
    {
        if (texture)
            SDL_DestroyTexture(texture);
        if (renderer)
            SDL_DestroyRenderer(renderer);
        if (window)
            SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void run()
    {
        cout << "Starting video loop..." << endl;
        cout << "Controls:" << endl;
        cout << "  G - Grayscale toggle" << endl;
        cout << "  ESC - Exit" << endl;
        cout << "  Space - Save frame" << endl;

        Mat frame, display_frame;
        bool running = true;
        bool grayscale = false;
        int frame_count = 0;
        int save_count = 0;

        Uint32 last_time = SDL_GetTicks();

        while (running)
        {
            // Handle events
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    running = false;
                    break;

                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                    case SDLK_ESCAPE:
                        running = false;
                        break;
                    case SDLK_g:
                        grayscale = !grayscale;
                        cout << "Grayscale: " << (grayscale ? "ON" : "OFF") << endl;
                        break;
                    case SDLK_SPACE:
                        if (!frame.empty())
                        {
                            string filename = "sdl_frame_" + to_string(++save_count) + ".jpg";
                            imwrite(filename, display_frame);
                            cout << "Frame saved: " << filename << endl;
                        }
                        break;
                    }
                    break;
                }
            }

            // Capture frame
            if (cap.read(frame) && !frame.empty())
            {
                frame_count++;

                // Process frame
                if (grayscale)
                {
                    cvtColor(frame, display_frame, COLOR_BGR2GRAY);
                    cvtColor(display_frame, display_frame, COLOR_GRAY2BGR);
                }
                else
                {
                    display_frame = frame.clone();
                }

                // Convert BGR to RGB for SDL
                cvtColor(display_frame, display_frame, COLOR_BGR2RGB);

                // Add FPS counter
                Uint32 current_time = SDL_GetTicks();
                if (current_time - last_time >= 1000)
                {
                    cout << "Frame: " << frame_count << " | FPS: ~" << (frame_count * 1000.0 / (current_time - last_time + 1000)) << endl;
                    last_time = current_time;
                }

                // Update texture
                void *pixels;
                int pitch;
                SDL_LockTexture(texture, NULL, &pixels, &pitch);
                memcpy(pixels, display_frame.data, width * height * 3);
                SDL_UnlockTexture(texture);

                // Render
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
            }
            else
            {
                cout << "Failed to capture frame!" << endl;
                SDL_Delay(100);
            }

            // Small delay to prevent 100% CPU usage
            SDL_Delay(16); // ~60 FPS limit
        }

        cout << "Total frames processed: " << frame_count << endl;
    }
};

int main(int argc, char *argv[])
{
    cout << "=== SDL2 WEBCAM VIEWER ===" << endl;

    try
    {
        SDLWebcam viewer("http://192.168.1.133:4747/video");
        viewer.run();
    }
    catch (const exception &e)
    {
        cout << "ERROR: " << e.what() << endl;
        return -1;
    }

    cout << "Program ended successfully!" << endl;
    return 0;
}