#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
using namespace std;
using namespace cv;

const float hue_ranges[] = { 0, 180 }; // Область оттенка (оставляем только RGB)
const float saturation_ranges[] = { 0, 256 }; // Область насыщенности
const float* ranges[] = { hue_ranges, saturation_ranges }; // вместе

struct Image
{
    String filename;
    Mat histogram;
    Image(String fn) : filename(fn)
    {
        calculate_histogram(imread(fn));
    };

    void calculate_histogram(Mat img)
    {
        Mat hsv;
        cvtColor(img, hsv, COLOR_BGR2HSV); // Конвертируем из RGB (красный, зеленый, синий) в HSV (оттенок, насыщенность, значение)
        int h_bins = 50, s_bins = 60;
        int histSize[] = { h_bins, s_bins }; //Размеры хистограмы по осям оттенков и насыщенности
        int channels[] = { 0, 1 };
        calcHist(&img, 1, channels, Mat(), histogram, 2, histSize, ranges, true, false);// Вычисляем хистограму в пространстве оттеноков и насыщенности
        normalize(histogram, histogram, 0, 1, NORM_MINMAX, -1, Mat()); // Проводим нормализацию хистограммы
    }
};

void compare_images(Image img1, Image img2, short threshold)
{

    auto likeness = (short)(abs(compareHist(img1.histogram, img2.histogram, HistCompMethods::HISTCMP_CORREL)) * 100); // сравниваем хистограммы
    if (likeness >= threshold) cout << img1.filename << " " << img2.filename << " " << likeness << "%\n";
}





std::vector<String> parse_input()
{
    std::cout << "Input images' filenames:" << std::endl;
    std::vector<String> inputs;
    inputs.reserve(10);
    String line;
    std::getline(std::cin, line);
    line = "00";
    while (std::getline(std::cin, line) && !line.empty())
    {
        inputs.push_back(line);
    }
    return inputs;
}





void main()
{
    cv::utils::logging::setLogLevel(utils::logging::LogLevel::LOG_LEVEL_SILENT);
    short threshold;
    std::cout << "Enter the threshold:" << std::endl;
    std::cin >> threshold;
    auto inputs = parse_input();

    std::vector<Image> images;
    images.reserve(inputs.size());
    String blank;
    for (auto& input : inputs)
    {
        images.push_back(Image(inputs.back()));
        inputs.pop_back();
    }

    for (int i = 0; i < images.size(); i++)
        for (int j = i + 1; j < images.size(); j++)
            compare_images(images[i], images[j], threshold);


}