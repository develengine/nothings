#include "math.hpp"

int main(int argc, char *argv[]) {

    eng::Complex num1(1.3, 0.5);
    eng::Complex num2 = num1 * 4;
    eng::Complex num3 = num2 / 4;

    std::cout << num1 << '\n';
    std::cout << num2 << '\n';
    std::cout << num3 << '\n';
    std::cout << num2.length() << '\n';


    eng::Vector<5, float> v1(1.f, 2.f, 3.f, 4.f, 5.f);
    eng::Vector<5, float> v2(1.f, 2.f, 3.f, 4.f, 5.f);
    eng::Vector<5, float> v3 = v1 - v2;

    std::cout << v3 << '\n';

    eng::Vec2f v4(1.f, 0.f);
    eng::Vec2f v5(0.f, 1.f);

    std::cout << v4 * v5 << '\n';

    eng::Matrix4x4<float> mat1(1.0f);
    eng::Mat4f mat2 = mat1;
    mat2[0][0] = 0.6f;

    eng::Mat3f mat3(
        4.f, 0.f, 0.f,
        2.f, 8.f, 1.f,
        0.f, 1.f, 0.f
    );

    eng::Mat3f mat4(
        4.f, 2.f, 9.f,
        2.f, 0.f, 4.f,
        1.f, 4.f, 2.f
    );

    eng::Mat3f mat5 = mat3 * mat4;

    std::cout << mat1 << '\n' << mat2 << '\n'<< mat1 << '\n';
    std::cout << mat5;

    eng::Complex num4(3, 5);
    std::cout << num4 << '\n';
    std::cout << num4.length() << '\n';
    std::cout << num4.normalize() << '\n';
    std::cout << num4.normalize().length() << '\n';

    eng::Quaternion quat1(0.25, 0.25, 0.25, 0.25);
    eng::Quaternion quat2(4, 0, 0, 0);
    eng::Complex num5(4, 0);
    std::cout << (quat1 * quat2) << '\n';
    std::cout << (quat1 * num5) << '\n';
    std::cout << (quat1 * 4) << '\n';

    return 0;
}
