#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;
//(1) Declare a Header struct
struct Header {
//(2) Declare each Header element
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;
};
//(3) Declare a Pixel struct
struct Pixel {
//(4) Declare each Pixel element
    unsigned char blue; // unsigned char is a 8bit integer 0-255
    unsigned char green; // 0 (dark) - 255 (bright)
    unsigned char red;

    float floatBlue;
    float floatGreen;
    float floatRed;

    Pixel(){}

    Pixel(unsigned char red, unsigned char green, unsigned char blue);
    bool operator==(const Pixel &rhs) const;
    Pixel operator*(const Pixel &rhs);

    Pixel operator-(const Pixel &rhs);

    Pixel operator/(const Pixel &rhs);

};

//(5) Declare an Image class



class Image {
//(6) Declare private data member of the Image class
private:
//(7) Declare a name
//(8) Declare a stream (ifstream)
    ifstream stream;

public:
//(9) Declare a header for the Image
    Header header;
//(10) Declare a vector of pixels
    vector<Pixel> pixelVect;
//(11) Declare an Image constructor
    Image();
    Image oneColor(char channel);

    //(12) Declare a read function
    void read(const string& inName);
//(13) Declare a write function
    void write(const string& outName);
    string name;

    Image(const Image& other);
    Image& operator=(const Image& other);

    Image flipImage();

    Image addBlue(const int &k);
    Image addGreen(const int &k);
    Image addRed(const int &k);

    Image scaleBlue(const int &k);
    Image scaleGreen(const int &k);
    Image scaleRed(const int &k);

};
