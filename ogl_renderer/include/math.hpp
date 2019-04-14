#pragma once

#include <iostream>
#include <array>

#include <math.h>

#define ENG_MATH_GL

namespace eng {

    template<typename T>
    class ComplexNumber {

    public:

        T r, i;

        ComplexNumber(T r, T i) : r(r), i(i) { }

        inline T length() const {
            return sqrt(r * r + i * i);
        }

        inline T angle() const {
            return atan((i / r));
        }

        inline ComplexNumber normalize() const {
            return (*this) * (1 / this->length());
        }

        inline ComplexNumber operator+(const ComplexNumber &b) const {
            return ComplexNumber(r + b.r, i + b.i);
        }

        inline ComplexNumber operator+(T b) const {
            return ComplexNumber(r + b, i);
        }

        inline ComplexNumber operator-(const ComplexNumber &b) const {
            return ComplexNumber(r - b.r, i - b.i);
        }

        inline ComplexNumber operator-(const T &b) const {
            return ComplexNumber(r - b, i);
        }

        inline ComplexNumber operator*(const ComplexNumber &b) const {
            return ComplexNumber(r * b.r - i * b.i, r * b.i + i * b.r);
        }

        inline ComplexNumber operator*(T b) const {
            return ComplexNumber(r * b, i * b);
        }

        inline ComplexNumber operator/(const ComplexNumber &b) const {
            T d = b.r * b.r + b.i * b.i;
            return ComplexNumber(
                (r * b.r + i * b.i) / d,
                (i * b.r - r * b.i) / d
            );
        }

        inline ComplexNumber operator/(T b) const {
            T d = b * b;
            return ComplexNumber((r * b) / d, (i * b) / d);
        }

        friend std::ostream& operator<<(std::ostream &stream, const ComplexNumber &a) {
            stream << a.r << (a.i >= 0 ? " + " : " - ") << abs(a.i) << 'i';
            return stream;
        }

    };



    template<int D, typename T>
    class Vector {
        static_assert(D > 0, "Number of dimensions must be greater than 0");

    protected:

        T data[D];

    public:

        template <typename... Args>
        Vector(Args... args) : data{ T(args)... } {
            static_assert(sizeof...(Args) == D, "Wrong number of arguments");
        }

        Vector() { }


        int getDimensions() const {
            return D;
        }

        inline T length() const {
            T output = 0;
            for (int i = 0; i < D; i++)
                output += data[i] * data[i];
            return sqrt(output);
        }

        inline Vector normalize() {
            Vector output;
            T d = 1 / this->length();
            for (int i = 0; i < D; i++)
                output[i] = data[i] * d;
            return output;
        }

        inline T operator[](int i) const {
            return data[i];
        }

        inline T & operator[](int i) {
            return data[i];
        }

        inline Vector operator+(const Vector &b) const {
            Vector output = Vector();
            for (int i = 0; i < D; i++)
                output[i] = (*this)[i] + b[i];
            return output;
        }

        inline Vector operator-(const Vector &b) const {
            Vector output = Vector();
            for (int i = 0; i < D; i++)
                output[i] = (*this)[i] - b[i];
            return output;
        }

        inline T operator*(const Vector &b) const {
            T output = 0;
            for (int i = 0; i < D; i++)
                output += (*this)[i] * b[i];
            return output;
        }

        inline Vector operator*(T b) const {
            Vector output = Vector();
            for (int i = 0; i < D; i++)
                output[i] = (*this)[i] * b;
            return output;
        }

        friend std::ostream& operator<<(std::ostream &stream, const Vector &a) {
            stream << "[ " << a[0];
            for (int i = 1; i < D; i++)
                stream << ", "  << a[i];
            stream << " ]";
            return stream;
        }

    };


    template<typename T>
    class Vector3 : public Vector<3, T> {

    public:

        template <typename... Args>
        Vector3(Args... args) : Vector<3, T>(args...) { }

        Vector3() { }

        inline Vector3 cross(const Vector3 &b) const {
            const Vector3 &a = (*this);
            return Vector3(
                a[1] * b[2] - a[2] * b[1],
                a[2] * b[0] - a[0] * b[2],
                a[0] * b[1] - a[1] * b[0]
            );
        }

    };


    template<typename T>
    class QuaternionNumber {

    public:

        T r, i, j, k;

        QuaternionNumber(T r, T i, T j, T k) :
            r(r), i(i), j(j), k(k) { }
        
        QuaternionNumber(T angle, Vector<3, T> axis) {
            T sin_val = sin(angle / 2);
            r = cos(angle / 2);
            i = axis[0] * sin_val;
            j = axis[1] * sin_val;
            k = axis[2] * sin_val;
        }

        QuaternionNumber() { }

        inline T length() const {
            return sqrt(r * r + i * i + j * j + k * k);
        }

        inline QuaternionNumber normalize() const {
            return (*this) * (1 / this->length());
        }

        inline QuaternionNumber operator+(const QuaternionNumber &b) const {
            return QuaternionNumber(r + b.r, i + b.i, j + b.j, k + b.k);
        }

        inline QuaternionNumber operator-(const QuaternionNumber &b) const {
            return QuaternionNumber(r - b.r, i - b.i, j - b.j, k - b.k);
        }

        inline QuaternionNumber operator*(const QuaternionNumber &b) const {
            return QuaternionNumber(
                r * b.r - i * b.i - j * b.j - k * b.k,
                r * b.i + i * b.r + j * b.k - k * b.j,
                r * b.j - i * b.k + j * b.r + k * b.i,
                r * b.k + i * b.j - j * b.i + k * b.r
            );
        }

        template<typename CT>
        inline QuaternionNumber operator*(const ComplexNumber<CT> &b) const {
            return QuaternionNumber(
                r * b.r - i * b.i,
                r * b.i + i * b.r,
                j * b.r + k * b.i,
                j * b.i + k * b.r
            );
        }

        inline QuaternionNumber operator*(T b) const {
            return QuaternionNumber(
                r * b, i * b, j * b, k * b
            );
        }

        friend std::ostream& operator<<(std::ostream &stream, const QuaternionNumber &a) {
            stream << a.r << (a.i >= 0 ? " + " : " - ") << abs(a.i) << 'i'
                          << (a.j >= 0 ? " + " : " - ") << abs(a.j) << 'j'
                          << (a.k >= 0 ? " + " : " - ") << abs(a.k) << 'k';
            return stream;
        }

    };



    template<int D, typename T>
    class Matrix {
        static_assert(D > 0, "Number of dimensions must be greater than 0");

    protected:

        T data[D * D];

    public:

        Matrix(T value) {
            for (int i = 0; i < D * D; i++)
                data[i] = 0;
            for (int i = 0; i < D; i++)
                data[i * D + i] = value;
        }

        // template <typename... Args>
        // Matrix(Args... args) : data{ T(args)... } {
        //     static_assert(sizeof...(Args) == D * D, "Wrong number of arguments");
        // }

        Matrix() { }


        inline const T* operator[](int i) const {
            return &(data[i * D]);
        }

        inline T* operator[](int i) {
            return &(data[i * D]);
        }

        inline Matrix operator*(const Matrix &b) const {
            Matrix output = Matrix();
            for (int i = 0; i < D; i++) {
                for (int j = 0; j < D; j++) {
                    T value = 0;
                    for (int k = 0; k < D; k++)
                        value += b[j][k] * (*this)[k][i];
                    output[j][i] = value;
                }
            }
            return output;
        }

        template<typename VT>
        inline Vector<D, T> operator*(const Vector<D, VT> &v) const {
            Vector<D, VT> output;
            for (int i = 0; i < D; i++) {
                T sum = 0;
                for (int j = 0; j < D; j++)
                    sum += (*this)[j][i] * v[j];
                output[i] = sum;
            }
            return output;
        }

        friend std::ostream& operator<<(std::ostream &stream, Matrix &a) {
            for (int j = 0; j < D; j++) {
                stream << "[ " << (a[j][0]);
                for (int i = 1; i < D; i++)
                    stream << ", "  << (a[j][i]);
                stream << " ]\n";
            }
            return stream;
        }

    };


    template<typename T>
    class Matrix4x4 : public Matrix<4, T> {

    public:

        // Matrix4x4(T value) : Matrix<4, T>(value) { }

        // // template<typename... Args>
        // // Matrix4x4(Args... args) : Matrix<4, T>(args...) { }

        // Matrix4x4() { }


        static Matrix<4, T> translation(T x, T y, T z) {
            Matrix<4, T> output = Matrix<4, T>(1);
            output[3][0] = x;
            output[3][1] = y;
            output[3][2] = z;
            return output;
        }

        static Matrix<4, T> scale(T x, T y, T z) {
            Matrix<4, T> output = Matrix<4, T>(0);
            output[0][0] = x;
            output[1][1] = y;
            output[2][2] = z;
            return output;
        }

        static Matrix<4, T> xRotation(T angle) {
            T si = sin(angle);
            T co = cos(angle);
            Matrix<4, T> output = Matrix<4, T>(1);
            output[1][1] = co;
            output[2][1] = -si;
            output[1][2] = si;
            output[2][2] = co;
            return output;
        }

        static Matrix<4, T> yRotation(T angle) {
            T si = sin(angle);
            T co = cos(angle);
            Matrix<4, T> output = Matrix<4, T>(1);
            output[0][0] = co;
            output[0][2] = -si;
            output[2][0] = si;
            output[2][2] = co;
            return output;
        }

        static Matrix<4, T> zRotation(T angle) {
            T si = sin(angle);
            T co = cos(angle);
            Matrix<4, T> output = Matrix<4, T>(1);
            output[0][0] = co;
            output[1][0] = -si;
            output[0][1] = si;
            output[1][1] = co;
            return output;
        }

#ifdef ENG_MATH_GL

        static Matrix<4, T> GL_Projection(
            float fov, float width, float height,
            float nPlane, float fPlane
        ) {
            float aspectRatio = width / height;
            float yScale = (float)((1.0f / tan((fov / 2.0f) * M_PI / 180)) * aspectRatio);
            float xScale = yScale / aspectRatio;
            float frustumLength = fPlane - nPlane;
            Matrix<4, T> output = Matrix<4, T>(1);
            output[0][0] = xScale;
            output[1][1] = yScale;
            output[2][2] = -((fPlane + nPlane) / frustumLength);
            output[2][3] = -1;
            output[3][2] = -((2 * nPlane * fPlane) / frustumLength);
            return output;
        }

        template<typename QT>
        Matrix<4, T> GL_View(
            float x, float y, float z,
            const QuaternionNumber<QT> &q
        ) {
            return Matrix<4, T>();
	    }


#endif

        template<typename QT>
        static Matrix4x4 rotation(const QuaternionNumber<QT> &q) {
            T ii = q.i * q.i;
            T ij = q.i * q.j;
            T ik = q.i * q.k;
            T ir = q.i * q.r;
            T jj = q.j * q.j;
            T jk = q.j * q.k;
            T jr = q.j * q.r;
            T kk = q.k * q.k;
            T kr = q.k * q.r;
            return Matrix4x4(
                1 - 2 * (jj + kk), 2 * (ij - kr), 2 * (ik + jr), 0,
                2 * (ij + kr), 1 - 2 * (ii + kk), 2 * (jk - ir), 0,
                2 * (ik - jr), 2 * (jk + ir), 1 - 2 * (ii * jj), 0,
                0, 0, 0, 1
            );
        }

    };


    using Complex = ComplexNumber<double>;
    using Quaternion = QuaternionNumber<double>;
    using Complexf = ComplexNumber<float>;
    using Quaternionf = QuaternionNumber<float>;

    using Vec2f = Vector<2, float>;
    using Vec3f = Vector3<float>;
    using Vec4f = Vector<4, float>;
    using Vec2 = Vector<2, double>;
    using Vec3 = Vector3<double>;
    using Vec4 = Vector<4, double>;

    using Mat2f = Matrix<2, float>;
    using Mat3f = Matrix<3, float>;
    using Mat4f = Matrix<4, float>;
    using Mat2 = Matrix<2, double>;
    using Mat3 = Matrix<3, double>;
    using Mat4 = Matrix<4, double>;

}
