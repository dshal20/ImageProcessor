#include <iostream>
#include <ostream>
#include <fstream>
#include <vector>
#include "image.h"
#include <cmath>
#include <cstring>
#include <sstream>
using namespace std;

Image combineLayers(Image &red, Image &green, Image &blue) {
    Image endImage;
    endImage.header = red.header;
    for (unsigned int i = 0; i < red.pixelVect.size(); i++) {
        Pixel pixel;
        pixel.red = red.pixelVect[i].red;
        pixel.green = green.pixelVect[i].green;
        pixel.blue = blue.pixelVect[i].blue;
        endImage.pixelVect.push_back(pixel);
    }
    return endImage;
}

bool checkTGA(const string& input){
    string tga = ".tga";
    if(input.substr(input.size() - 4) == tga){
        return true;
    }
    else{
        return false;
    }
}

unsigned char Clamp(int value) {
    if (value < 0) {
        return 0;
    } else if (value > 255) {
        return 255;
    } else {
        return static_cast<unsigned char>(value);
    }
}

float Normalize(int value) {
    return static_cast<float>(value) / 255.0f;
}

Image MultiplyBlending(Image &bottomLayer, Image &topLayer) {
    for (size_t i = 0; i < bottomLayer.pixelVect.size(); ++i) {
        Pixel &bottomPixel = bottomLayer.pixelVect[i];
        Pixel &topPixel = topLayer.pixelVect[i];

        float normalizedBottomR = Normalize(bottomPixel.red);
        float normalizedBottomG = Normalize(bottomPixel.green);
        float normalizedBottomB = Normalize(bottomPixel.blue);

        float normalizedTopR = Normalize(topPixel.red);
        float normalizedTopG = Normalize(topPixel.green);
        float normalizedTopB = Normalize(topPixel.blue);

        bottomPixel.red = static_cast<unsigned char>(round(normalizedBottomR * normalizedTopR * 255));
        bottomPixel.green = static_cast<unsigned char>(round(normalizedBottomG * normalizedTopG * 255));
        bottomPixel.blue = static_cast<unsigned char>(round(normalizedBottomB * normalizedTopB * 255));
    }
    return bottomLayer;
}

Image ScreenBlending(Image &bottom, Image &top) {
    for (size_t i = 0; i < bottom.pixelVect.size(); ++i) {
        Pixel &bottomPixel = bottom.pixelVect[i];
        Pixel &topPixel = top.pixelVect[i];

        float normalizedBottomR = Normalize(bottomPixel.red);
        float normalizedBottomG = Normalize(bottomPixel.green);
        float normalizedBottomB = Normalize(bottomPixel.blue);

        float normalizedTopR = Normalize(topPixel.red);
        float normalizedTopG = Normalize(topPixel.green);
        float normalizedTopB = Normalize(topPixel.blue);

        bottomPixel.red = static_cast<unsigned char>(round((1 - ((1 - normalizedBottomR) * (1 - normalizedTopR))) * 255));
        bottomPixel.green = static_cast<unsigned char>(round((1 - ((1 - normalizedBottomG) * (1 - normalizedTopG))) * 255));
        bottomPixel.blue = static_cast<unsigned char>(round((1 - ((1 - normalizedBottomB) * (1 - normalizedTopB))) * 255));
    }
    return bottom;
}

Image SubtractBlending(Image &bottom, Image &top) {
    for (size_t i = 0; i < bottom.pixelVect.size(); ++i) {
        Pixel &bottomPixel = bottom.pixelVect[i];
        Pixel &topPixel = top.pixelVect[i];

        int newRed = static_cast<int>(topPixel.red) - static_cast<int>(bottomPixel.red);
        int newGreen = static_cast<int>(topPixel.green) - static_cast<int>(bottomPixel.green);
        int newBlue = static_cast<int>(topPixel.blue) - static_cast<int>(bottomPixel.blue);

        bottomPixel.red = static_cast<unsigned char>(round(Clamp(newRed)));
        bottomPixel.green = static_cast<unsigned char>(round(Clamp(newGreen)));
        bottomPixel.blue = static_cast<unsigned char>(round(Clamp(newBlue)));
    }
    return bottom;
}

Image AdditionBlending(Image &bottom, Image &top) {
    for (size_t i = 0; i < bottom.pixelVect.size(); ++i) {
        Pixel &bottomPixel = bottom.pixelVect[i];
        Pixel &topPixel = top.pixelVect[i];

        int newRed = static_cast<int>(bottomPixel.red) + static_cast<int>(topPixel.red);
        int newGreen = static_cast<int>(bottomPixel.green) + static_cast<int>(topPixel.green);
        int newBlue = static_cast<int>(bottomPixel.blue) + static_cast<int>(topPixel.blue);

        bottomPixel.red = static_cast<unsigned char>(Clamp(newRed));
        bottomPixel.green = static_cast<unsigned char>(Clamp(newGreen));
        bottomPixel.blue = static_cast<unsigned char>(Clamp(newBlue));
    }
    return bottom;
}

Image OverlayBlending(Image &bottom, Image &top) {
    for (size_t i = 0; i < bottom.pixelVect.size(); ++i) {
        Pixel &bottomPixel = bottom.pixelVect[i];
        Pixel &topPixel = top.pixelVect[i];

        float normalizedBottomR = Normalize(bottomPixel.red);
        float normalizedBottomG = Normalize(bottomPixel.green);
        float normalizedBottomB = Normalize(bottomPixel.blue);

        float normalizedTopR = Normalize(topPixel.red);
        float normalizedTopG = Normalize(topPixel.green);
        float normalizedTopB = Normalize(topPixel.blue);

        if (normalizedTopR <= 0.5) {
            bottomPixel.red = static_cast<unsigned char>(round(2 * normalizedBottomR * normalizedTopR * 255));
        } else {
            bottomPixel.red = static_cast<unsigned char>(round((1 - 2 * (1 - normalizedBottomR) * (1 - normalizedTopR)) * 255));
        }

        if (normalizedTopG <= 0.5) {
            bottomPixel.green = static_cast<unsigned char>(round(2 * normalizedBottomG * normalizedTopG * 255));
        } else {
            bottomPixel.green = static_cast<unsigned char>(round((1 - 2 * (1 - normalizedBottomG) * (1 - normalizedTopG)) * 255));
        }

        if (normalizedTopB <= 0.5) {
            bottomPixel.blue = static_cast<unsigned char>(round(2 * normalizedBottomB * normalizedTopB * 255));
        } else {
            bottomPixel.blue = static_cast<unsigned char>(round((1 - 2 * (1 - normalizedBottomB) * (1 - normalizedTopB)) * 255));
        }
    }
    return bottom;
}


int main(int argc, char* argv[]) {


    Image out;
    vector<string> args;

    for(int i = 1; i < argc; i++){
        args.push_back(argv[i]);
    }

    if ((argc == 1) || (strcmp(argv[1], "--help") == 0)) {
        cout << "Project 2: Image Processing, Fall 2023" << endl
             << "\n"
             << "Usage:" << "\n"
             << "\t./project2.out [output] [firstImage] [method] [...]";
        return 0;
    }
    else if (args[0].size() < 5) {
        cout << "Invalid file name." << endl;
        return 0;
    }
    else if (!checkTGA(args[0])) {
        cout << "Invalid file name." << endl;
        return 0;
    }
    else if (args[1].size() < 5) {
        cout << "Invalid file name." << endl;
        return 0;
    }
    else if(argc < 3){
        cout << "Missing argument." << endl;
        return 0;
    }
    if (args[1].size() < 5) {
        cout << "Invalid file name." << endl;
        return 0;
    }
    else if (!checkTGA(args[1])) {
        cout << "Invalid file name." << endl;
        return 0;
    }
    fstream inFS;
    inFS.open(args[1]);
    if(!inFS.is_open()){
        cout << "File does not exist." << endl;
        return 0;
    }



    string sourceImageFilename = argv[1];
    Image trackingImage;
    trackingImage.read(args[1]);
    trackingImage.name = args[1];


    for (int i = 3; i < argc; i++) {


        if(strcmp(argv[i], "multiply") == 0){
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }
            if(!checkTGA(argv[i + 1])) {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            Image second;
            second.read(argv[i + 1]);
            second.name = argv[i + 1];
            cout << "Multiplying " << trackingImage.name << " and " << second.name <<  " ..." << endl;
            MultiplyBlending(trackingImage, second);
            Image temp = MultiplyBlending(trackingImage, second);
            trackingImage = temp;
            i++;
        }

        else if (strcmp(argv[i], "subtract") == 0) {
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }
            if(!checkTGA(argv[i + 1])) {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            Image second;
            second.read(argv[i + 1]);
            second.name = argv[i + 1];
            cout << "Subtracting " << trackingImage.name << " and " << second.name <<  " ..." << endl;
            Image temp = SubtractBlending(second, trackingImage);
            trackingImage = temp;
            i++;
        }
        else if (strcmp(argv[i], "overlay") == 0) {
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }
            if(!checkTGA(argv[i + 1])) {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            Image second;
            second.read(argv[i + 1]);
            second.name = argv[i + 1];
            Image temp = OverlayBlending(trackingImage, second);
            trackingImage = temp;
            i++;
        }
        else if (strcmp(argv[i], "screen") == 0) {
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }
            if(!checkTGA(argv[i + 1])) {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            Image second;
            second.read(argv[i + 1]);
            second.name = argv[i + 1];
            Image temp = ScreenBlending(trackingImage, second);
            trackingImage = temp;
            i++;
        }

        else if (strcmp(argv[i], "combine") == 0) {
            if(i + 2 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }
            if(!checkTGA(argv[i + 1])) {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            if(!checkTGA(argv[i + 2])) {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            Image second;
            second.read(argv[i + 1]);
            Image third;
            third.read(argv[i + 2]);
            second.name = argv[i + 1];
            third.name = argv[i + 2];
            cout << "Combining channels of " << trackingImage.name << ", " << second.name << ", and " << third.name << endl;
            Image temp = combineLayers(trackingImage,second,third);
            trackingImage = temp;
            i += 2;
        }
        else if(strcmp(argv[i], "flip") == 0){
            Image temp = trackingImage.flipImage();
            trackingImage = temp;
        }



        else if(strcmp(argv[i], "onlyred") == 0){
            Image temp = trackingImage.oneColor('r');
            trackingImage = temp;
        }
        else if(strcmp(argv[i], "onlyblue") == 0){
            Image temp = trackingImage.oneColor('b');
            trackingImage = temp;
        }
        else if(strcmp(argv[i], "onlygreen") == 0){
            Image temp = trackingImage.oneColor('g');
            trackingImage = temp;
        }



        else if(strcmp(argv[i], "addred") == 0){
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }
            int num;
            try {
                num = stoi(argv[i + 1]);
            }
            catch (const invalid_argument) {
                cout << "Invalid argument, expected number." << endl;
                return 0;
            }
            Image temp = trackingImage.addRed(num);
            trackingImage = temp;
            i++;
        }
        else if(strcmp(argv[i], "addgreen") == 0){
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }
            int num;
            try {
                num = stoi(argv[i + 1]);
            }
            catch (const invalid_argument) {
                cout << "Invalid argument, expected number." << endl;
                return 0;
            }
            Image temp = trackingImage.addGreen(num);
            trackingImage = temp;
            i++;
        }
        else if(strcmp(argv[i], "addblue") == 0){
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }

            int num;
            try {
                num = stoi(argv[i + 1]);
            }
            catch (const invalid_argument) {
                cout << "Invalid argument, expected number." << endl;
                return 0;
            }
            Image temp = trackingImage.addBlue(num);
            trackingImage = temp;
            i++;
        }


        else if(strcmp(argv[i], "scalered") == 0){
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }

            int num;
            try {
                num = stoi(argv[i + 1]);
            }
            catch (const invalid_argument) {
                cout << "Invalid argument, expected number." << endl;
                return 0;
            }

            Image temp = trackingImage.scaleRed(num);
            trackingImage = temp;
            i++;
        }
        else if(strcmp(argv[i], "scalegreen") == 0){
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }

            int num;
            try {
                num = stoi(argv[i + 1]);
            }
            catch (const invalid_argument) {
                cout << "Invalid argument, expected number." << endl;
                return 0;
            }

            Image temp = trackingImage.scaleGreen(num);
            trackingImage = temp;
            i++;
        }
        else if(strcmp(argv[i], "scaleblue") == 0){
            if(i + 1 >= argc){
                cout << "Missing argument." << endl;
                exit(0);
            }

            int num;
            try {
                num = stoi(argv[i + 1]);
            }
            catch (const invalid_argument) {
                cout << "Invalid argument, expected number." << endl;
                return 0;
            }

            Image temp = trackingImage.scaleBlue(num);
            trackingImage = temp;
            i++;
        }


        else{
            cout << "Invalid method name." << endl;
            return 0;
        }

    }

    string outputFilename = argv[1];
    cout << "... and saving output to " << outputFilename << "!" << endl;
    trackingImage.write(outputFilename);
    return 0;


}
