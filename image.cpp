#include "image.h"
#include <sstream>
#include <iostream>
#include <cmath>
using namespace std;

Pixel::Pixel(unsigned char red, unsigned char green, unsigned char blue){
    this->blue = blue;
    this->green = green;
    this->red = red;
    floatBlue = (float)blue / 255;
    floatGreen = (float)green / 255;
    floatRed = (float)red / 255;
}

bool Pixel::operator==(const Pixel &rhs) const{
    if(blue == rhs.blue && green == rhs.green && red == rhs.red)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Pixel Pixel::operator - (const Pixel &rhs) {
    unsigned char redChange;
    unsigned char greenChange;
    unsigned char blueChange;
    if ((int)this->red > (int)rhs.red) {
        redChange = (int)this->red - (int)rhs.red;
    }
    else {
        redChange = 0;
    }
    if ((int)this->green > (int)rhs.green) {
        greenChange = (int)this->green - (int)rhs.green;
    }
    else {
        greenChange = 0;
    }
    if ((int)this->blue > (int)rhs.blue) {
        blueChange = (int)this->blue - (int)rhs.blue;
    }
    else {
        blueChange = 0;
    }

    return Pixel(redChange, greenChange, blueChange);
}
Pixel Pixel::operator*(const Pixel &rhs){
    return Pixel((this->floatRed * rhs.floatRed) * 255 + 0.5f,
                 (this->floatGreen * rhs.floatGreen) * 255 + 0.5f,
                 (this->floatBlue * rhs.floatBlue) * 255 + 0.5f);
}
Pixel Pixel::operator/(const Pixel &rhs) {
    float newRed = (1 - (1 - this->floatRed) * (1 - rhs.floatRed)) * 255 + 0.5f;
    float newGreen = (1 - (1 - this->floatGreen) * (1 - rhs.floatGreen)) * 255 + 0.5f;
    float newBlue = (1 - (1 - this->floatBlue) * (1 - rhs.floatBlue)) * 255 + 0.5f;
    return Pixel((unsigned char)newRed, (unsigned char)newGreen, (unsigned char)newBlue);
}



Image::Image() {
}

void Image::read(const string& inName) {
    this->stream = ifstream(inName, ios_base::binary);
    if(!stream.is_open()) {
        cout << "Invalid argument, file does not exist." << endl;
        exit(0);
    }
    else {

        stream.read((char *) &header.idLength, sizeof(header.idLength));
        stream.read((char *) &header.colorMapType, sizeof(header.colorMapType));
        stream.read((char *) &header.dataTypeCode, sizeof(header.dataTypeCode));
        stream.read((char *) &header.colorMapOrigin, sizeof(header.colorMapOrigin));
        stream.read((char *) &header.colorMapLength, sizeof(header.colorMapLength));
        stream.read((char *) &header.colorMapDepth, sizeof(header.colorMapDepth));
        stream.read((char *) &header.xOrigin, sizeof(header.xOrigin));
        stream.read((char *) &header.yOrigin, sizeof(header.yOrigin));
        stream.read((char *) &header.width, sizeof(header.width));
        stream.read((char *) &header.height, sizeof(header.height));
        stream.read((char *) &header.bitsPerPixel, sizeof(header.bitsPerPixel));
        stream.read((char *) &header.imageDescriptor, sizeof(header.imageDescriptor));
        int imageSize = (int) header.height * header.width;
        for (int i = 0; i < imageSize; i++) {
            Pixel pixel;
            stream.read((char *) &pixel.blue, sizeof(pixel.blue));
            stream.read((char *) &pixel.green, sizeof(pixel.green));
            stream.read((char *) &pixel.red, sizeof(pixel.red));
            pixelVect.push_back(pixel);
        }
    }
}

void Image::write(const string& outName) {
    ofstream stream(outName, ios_base::binary);
    stream.write((char*)&header.idLength , sizeof(header.idLength));
    stream.write((char*)&header.colorMapType , sizeof(header.colorMapType));
    stream.write((char*)&header.dataTypeCode , sizeof(header.dataTypeCode));
    stream.write((char*)&header.colorMapOrigin , sizeof(header.colorMapOrigin));
    stream.write((char*)&header.colorMapLength , sizeof(header.colorMapLength));
    stream.write((char*)&header.colorMapDepth , sizeof(header.colorMapDepth));
    stream.write((char*)&header.xOrigin , sizeof(header.xOrigin));
    stream.write((char*)&header.yOrigin, sizeof(header.yOrigin));
    stream.write((char*)&header.width , sizeof(header.width));
    stream.write((char*)&header.height , sizeof(header.height));
    stream.write((char*)&header.bitsPerPixel , sizeof(header.bitsPerPixel));
    stream.write((char*)&header.imageDescriptor , sizeof(header.imageDescriptor));
    for(int i = 0; i < pixelVect.size(); i++) {
        Pixel pixel = pixelVect[i];
        stream.write((char*)&pixel.blue, sizeof(pixel.blue));
        stream.write((char*)&pixel.green, sizeof(pixel.green));
        stream.write((char*)&pixel.red, sizeof(pixel.red));
    }
}
Image Image::oneColor(char channel) {
    Image result;
    result.header = header;

    for (const Pixel& pixel : pixelVect) {
        Pixel onePixel;
        if (channel == 'r') {
            onePixel.red = pixel.red;
            onePixel.green = pixel.red;
            onePixel.blue = pixel.red;
        } else if (channel == 'g') {
            onePixel.red = pixel.green;
            onePixel.green = pixel.green;
            onePixel.blue = pixel.green;
        } else if (channel == 'b') {
            onePixel.red = pixel.blue;
            onePixel.green = pixel.blue;
            onePixel.blue = pixel.blue;
        }

        result.pixelVect.push_back(onePixel);
    }

    return result;
}

Image::Image(const Image& other) {
    header = other.header;
    pixelVect = other.pixelVect;
    name = other.name;

    if (other.stream.is_open()) {
        stream.open(name, std::ios_base::binary);
    }
}

Image& Image::operator=(const Image& other) {
    if (this != &other) {
        header = other.header;
        pixelVect = other.pixelVect;
        name = other.name;

        if (other.stream.is_open()) {
            stream.open(name, std::ios_base::binary);
        }
    }
    return *this;
}

Image Image::flipImage() {
    Image result = *this;
    vector<Pixel> tempVec(result.pixelVect.size());
    for (unsigned int i = 0; i < result.pixelVect.size(); i++) {
        tempVec[result.pixelVect.size() - i - 1] = result.pixelVect[i];
    }
    result.pixelVect = tempVec;

    return result;
}


Image Image::addRed(const int &k) {
    Image endImage = *this;
    for (unsigned int i = 0; i < endImage.pixelVect.size(); i++) {
        if ((int)endImage.pixelVect[i].red + k > 255) {
            endImage.pixelVect[i].red = 255;
        }
        else{
            endImage.pixelVect[i].red = (int)endImage.pixelVect[i].red + k;
        }
    }
    return endImage;
}

Image Image::addBlue(const int &k) {
    Image endImage = *this;
    for (unsigned int i = 0; i < endImage.pixelVect.size(); i++) {
        if ((int)endImage.pixelVect[i].blue + k > 255) {
            endImage.pixelVect[i].blue = 255;
        }
        else{
            endImage.pixelVect[i].blue = (int)endImage.pixelVect[i].blue + k;
        }
    }
    return endImage;
}
Image Image::addGreen(const int &k) {
    Image endImage = *this;
    for (unsigned int i = 0; i < endImage.pixelVect.size(); i++) {
        if ((int)endImage.pixelVect[i].green + k > 255) {
            endImage.pixelVect[i].green = 255;
        }
        else{
            endImage.pixelVect[i].green = (int)endImage.pixelVect[i].green + k;
        }
    }
    return endImage;
}


Image Image::scaleBlue(const int &k) {
    Image result = *this;
    for (unsigned int i = 0; i < result.pixelVect.size(); i++) {
        float newBlue = (result.pixelVect[i].floatBlue * k) * 255 + 0.5f;
        if (newBlue < 255) {
            result.pixelVect[i].blue = static_cast<unsigned char>(round(newBlue));
        } else {
            result.pixelVect[i].blue = 255;
        }
    }
    return result;
}
Image Image::scaleGreen(const int &k) {
    Image result = *this;
    for (unsigned int i = 0; i < result.pixelVect.size(); i++) {
        float newGreen = (result.pixelVect[i].floatGreen * k) * 255 + 0.5f;
        if (newGreen < 255) {
            result.pixelVect[i].green = static_cast<unsigned char>(round(newGreen));
        } else {
            result.pixelVect[i].green = 255;
        }
    }
    return result;
}
Image Image::scaleRed(const int &k) {
    Image result = *this;
    for (unsigned int i = 0; i < result.pixelVect.size(); i++) {
        float newRed = (result.pixelVect[i].floatRed * k) * 255 + 0.5f;
        if (newRed < 255) {
            result.pixelVect[i].red = static_cast<unsigned char>(round(newRed));
        } else {
            result.pixelVect[i].red = 255;
        }
    }
    return result;
}
