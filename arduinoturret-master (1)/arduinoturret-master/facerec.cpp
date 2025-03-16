#include "facerec.hpp"

using namespace cv;

int main(int argc, char** argv) {
    // Disable logging
    utils::logging::setLogLevel(utils::logging::LogLevel::LOG_LEVEL_SILENT);

    Mat videoStream;

    CascadeClassifier faceDetect;
    faceDetect.load("haarcascade_frontalface_default.xml");

    if (faceDetect.empty()) {
        std::cerr << "Cannot load cascade file.\n";
        system("pause");
        return 1;
    }

    // Init video capture
    VideoCapture capture;
    capture.open(0, CAP_DSHOW);

    if (!capture.isOpened()) {
        std::cerr << "Cannot open camera.\n";
        system("pause");
        return 2;
    }
    HANDLE serialPort = _openPort(6, 115200);

    if (!serialPort) {
        std::cerr << "Cannot open COM port.\n";
        system("pause");
        return 3;
    }

    // Create placeholder window
    namedWindow("facerec");

    std::string s;
    while (true) {
        capture >> videoStream;

        std::vector<Rect> faces;
        faceDetect.detectMultiScale(
            videoStream, faces, 1.05, 10, CASCADE_SCALE_IMAGE, Size(30, 30)
        );

        if (faces.size() > 0) {
            // Draw rectangle on detected face
            rectangle(
                videoStream, faces[0].tl(), faces[0].br(), Scalar(50, 50, 255),
                2
            );

            // Calculate the distance between crosshair and rectangle
            int x = faces[0].x;
            int y = faces[0].y;
            int width = faces[0].width;
            int height = faces[0].height;

            Point centrerect(x + width / 2, y + height / 2);
            Point crosshair(320, 240);

            short distance_x = centrerect.x - crosshair.x;
            short distance_y = crosshair.y - centrerect.y;

            s = '<' + std::to_string(distance_x) + 'Y' +
                std::to_string(distance_y) + '>';

            _sendData(serialPort, s);

            // Print the coordinates on the top left of the window
            putText(
                videoStream,
                std::to_string(distance_x) + ", " + std::to_string(distance_y),
                Point(10, 40), FONT_HERSHEY_DUPLEX, 1, Scalar(255, 255, 255), 1
            );
        }

        imshow("facerec", videoStream);

        char c = waitKey(10);
        if (c == 'f') {
            s = "<f>";
            _sendData(serialPort, s);
        } else if (c == 'm') {
            s = "<m>";
            _sendData(serialPort, s);
        } else if (c == 'q') {
            _closePort(serialPort);
            destroyAllWindows();
            return 0;
        }
    }
}
