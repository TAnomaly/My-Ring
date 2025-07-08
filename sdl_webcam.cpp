#include <iostream>
#include <opencv2/opencv.hpp>
#include <SDL2/SDL.h>
#include <vector>
#include <fstream>

using namespace std;
using namespace cv;

struct Button
{
    SDL_Rect rect;
    string text;
    bool hovered;
    bool pressed;

    Button(int x, int y, int w, int h, const string &t)
        : rect({x, y, w, h}), text(t), hovered(false), pressed(false) {}

    bool isClicked(int mouseX, int mouseY)
    {
        return mouseX >= rect.x && mouseX < rect.x + rect.w &&
               mouseY >= rect.y && mouseY < rect.y + rect.h;
    }
};

class SDLWebcam
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    VideoCapture cap;
    int width, height;

    // Face detection and recognition
    CascadeClassifier face_cascade;
    vector<Mat> saved_faces;
    vector<string> saved_names;
    bool has_saved_faces;

    // UI Elements
    Button save_face_button;
    Button clear_faces_button;

    // Face detection
    vector<Rect> detected_faces;
    Mat gray_frame;

public:
    SDLWebcam(const string &url) : save_face_button(10, 10, 120, 40, "Yuz Kaydet"),
                                   clear_faces_button(140, 10, 120, 40, "Temizle")
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
        window = SDL_CreateWindow("DroidCam Live - Yuz Tanima",
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  width, height + 60, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

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

        // Initialize face detection
        if (!face_cascade.load("haarcascade_frontalface_default.xml"))
        {
            cout << "UYARI: haarcascade_frontalface_default.xml yuklenemedi!" << endl;
        }

        // Initialize face recognition
        has_saved_faces = false;

        // Load saved faces if they exist
        loadSavedFaces();

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

    void loadSavedFaces()
    {
        ifstream file("saved_faces.dat", ios::binary);
        if (!file.is_open())
            return;

        int count;
        file.read(reinterpret_cast<char *>(&count), sizeof(count));

        for (int i = 0; i < count; i++)
        {
            // Load face data
            int rows, cols, type;
            file.read(reinterpret_cast<char *>(&rows), sizeof(rows));
            file.read(reinterpret_cast<char *>(&cols), sizeof(cols));
            file.read(reinterpret_cast<char *>(&type), sizeof(type));

            Mat face(rows, cols, type);
            file.read(reinterpret_cast<char *>(face.data), face.total() * face.elemSize());

            // Load name data
            int name_length;
            file.read(reinterpret_cast<char *>(&name_length), sizeof(name_length));

            string name(name_length, ' ');
            file.read(&name[0], name_length);

            saved_faces.push_back(face);
            saved_names.push_back(name);
        }

        file.close();

        if (!saved_faces.empty())
        {
            has_saved_faces = true;
            cout << "Kaydedilen " << saved_faces.size() << " yuz yuklendi!" << endl;
            for (size_t i = 0; i < saved_names.size(); i++)
            {
                cout << "  - " << saved_names[i] << endl;
            }
        }
    }

    void saveFacesToFile()
    {
        ofstream file("saved_faces.dat", ios::binary);
        if (!file.is_open())
            return;

        int count = saved_faces.size();
        file.write(reinterpret_cast<const char *>(&count), sizeof(count));

        for (size_t i = 0; i < saved_faces.size(); i++)
        {
            const Mat &face = saved_faces[i];
            const string &name = saved_names[i];

            // Save face data
            int rows = face.rows;
            int cols = face.cols;
            int type = face.type();

            file.write(reinterpret_cast<const char *>(&rows), sizeof(rows));
            file.write(reinterpret_cast<const char *>(&cols), sizeof(cols));
            file.write(reinterpret_cast<const char *>(&type), sizeof(type));
            file.write(reinterpret_cast<const char *>(face.data), face.total() * face.elemSize());

            // Save name data
            int name_length = name.length();
            file.write(reinterpret_cast<const char *>(&name_length), sizeof(name_length));
            file.write(name.c_str(), name_length);
        }

        file.close();
        cout << "Yuzler dosyaya kaydedildi!" << endl;
    }

    void detectFaces(const Mat &frame)
    {
        detected_faces.clear();

        if (face_cascade.empty())
            return;

        cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
        equalizeHist(gray_frame, gray_frame);

        face_cascade.detectMultiScale(gray_frame, detected_faces, 1.1, 3, 0, Size(30, 30));
    }

    void saveFace(const Mat &frame)
    {
        if (detected_faces.empty())
        {
            cout << "Yuz bulunamadi! Kameraya bakin ve tekrar deneyin." << endl;
            return;
        }

        // Get name from user
        cout << "\nYuz kaydediliyor..." << endl;
        cout << "Isim girin (bos birakabilirsiniz): ";
        string name;
        getline(cin, name);

        if (name.empty())
        {
            name = "Kisi_" + to_string(saved_faces.size() + 1);
        }

        // Save the first detected face
        Rect face_rect = detected_faces[0];
        Mat face_roi = gray_frame(face_rect);

        // Resize to standard size
        Mat resized_face;
        resize(face_roi, resized_face, Size(100, 100));

        saved_faces.push_back(resized_face.clone());
        saved_names.push_back(name);
        has_saved_faces = true;

        // Save to file
        saveFacesToFile();

        cout << "Yuz kaydedildi: " << name << " | Toplam: " << saved_faces.size() << endl;
    }

    void clearSavedFaces()
    {
        saved_faces.clear();
        saved_names.clear();
        has_saved_faces = false;

        // Delete the file
        remove("saved_faces.dat");

        cout << "Tum kaydedilen yuzler temizlendi!" << endl;
    }

    void drawButton(const Button &button, SDL_Color bg_color, SDL_Color text_color)
    {
        // Draw button background
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderFillRect(renderer, &button.rect);

        // Draw button border
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &button.rect);
    }

    void drawFaceRectangles(Mat &display_frame)
    {
        for (size_t i = 0; i < detected_faces.size(); i++)
        {
            Rect face = detected_faces[i];

            // Determine color based on recognition
            Scalar color = Scalar(0, 255, 0); // Green for unknown
            string label = "Bilinmeyen";

            if (has_saved_faces && !gray_frame.empty())
            {
                Mat face_roi = gray_frame(face);
                Mat resized_face;
                resize(face_roi, resized_face, Size(100, 100));

                // Simple template matching with saved faces
                double best_match = 0.0;
                int best_match_index = -1;

                for (size_t j = 0; j < saved_faces.size(); j++)
                {
                    Mat result;
                    matchTemplate(resized_face, saved_faces[j], result, TM_CCOEFF_NORMED);

                    double minVal, maxVal;
                    minMaxLoc(result, &minVal, &maxVal);

                    if (maxVal > best_match)
                    {
                        best_match = maxVal;
                        best_match_index = j;
                    }
                }

                if (best_match > 0.6 && best_match_index >= 0) // Threshold for recognition
                {
                    color = Scalar(0, 0, 255); // Red for known person
                    label = saved_names[best_match_index];
                }
            }

            // Draw rectangle
            rectangle(display_frame, face, color, 2);

            // Draw label
            putText(display_frame, label, Point(face.x, face.y - 10),
                    FONT_HERSHEY_SIMPLEX, 0.7, color, 2);
        }
    }

    void run()
    {
        cout << "Starting video loop..." << endl;
        cout << "Controls:" << endl;
        cout << "  G - Grayscale toggle" << endl;
        cout << "  ESC - Exit" << endl;
        cout << "  Space - Save frame" << endl;
        cout << "  Yuz Kaydet butonu - Gorunen yuzu isimle kaydet" << endl;
        cout << "  Temizle butonu - Kaydedilen yuzleri temizle" << endl;

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

                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT)
                    {
                        int mouseX = event.button.x;
                        int mouseY = event.button.y;

                        if (save_face_button.isClicked(mouseX, mouseY))
                        {
                            saveFace(frame);
                        }
                        else if (clear_faces_button.isClicked(mouseX, mouseY))
                        {
                            clearSavedFaces();
                        }
                    }
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

                // Detect faces
                detectFaces(frame);

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

                // Draw face rectangles and labels
                drawFaceRectangles(display_frame);

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

                // Render video
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);

                // Video area (leaving space at top for buttons)
                SDL_Rect video_rect = {0, 60, width, height};
                SDL_RenderCopy(renderer, texture, NULL, &video_rect);

                // Draw buttons
                SDL_Color button_bg = {100, 100, 100, 255};
                SDL_Color button_text = {255, 255, 255, 255};

                drawButton(save_face_button, button_bg, button_text);
                drawButton(clear_faces_button, button_bg, button_text);

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