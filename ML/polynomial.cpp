
#include "lib/SimpleMatrix.h"
#include <iostream>
#include <cstdlib>
#include <cmath>

using namespace std;

void weightInitialization(
    smat::Matrix<double>& weights,
    int rows, int cols) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            weights(i, j) = 0.0;
        }
    }
}

void biasInitialization(
    smat::Matrix<double>& bias,
    int rows, int cols) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            bias(i, j) = 0.0;
        }
    }
}

// Generate polynomial features
smat::Matrix<double> polynomialFeatures(
    const smat::Matrix<double>& X,
    int degree,
    int samples) {

    smat::Matrix<double> X_poly(samples, degree);

    for (int i = 0; i < samples; i++) {

        double x = X(i, 0);

        for (int j = 0; j < degree; j++) {
            X_poly(i, j) = pow(x, j + 1);
        }
    }

    return X_poly;
}

int main() {

    int samples = 5;
    int degree = 3;

    // Original input
    smat::Matrix<double> X(samples, 1);

    // Target output
    smat::Matrix<double> Y(samples, 1);

    double input[] = {
        -1.5, -0.5, 0.5, 1.5, 2.0
    };

    // y = 2 + 3x + 0.5x^2
    double output[] = {
        -0.875, 0.875, 3.125, 6.125, 10.0
    };

    for (int i = 0; i < samples; i++) {
        X(i, 0) = input[i];
        Y(i, 0) = output[i];
    }

    // Polynomial feature expansion
    smat::Matrix<double> X_poly =
        polynomialFeatures(X, degree, samples);

    // Model parameters
    smat::Matrix<double> W(degree, 1);
    smat::Matrix<double> B(1, 1);

    weightInitialization(W, degree, 1);
    biasInitialization(B, 1, 1);

    double learning_rate = 0.01;
    int epochs = 10000;

    for (int epoch = 0; epoch < epochs; epoch++) {

        // Forward propagation
        smat::Matrix<double> Y_hat = X_poly * W;

        for (int i = 0; i < samples; i++) {
            Y_hat(i, 0) += B(0, 0);
        }

        // Error
        smat::Matrix<double> E = Y_hat - Y;

        // Mean squared error
        double loss = 0.0;

        for (int i = 0; i < samples; i++) {
            loss += E(i, 0) * E(i, 0);
        }

        loss /= (2.0 * samples);

        // Gradient with respect to predictions
        smat::Matrix<double> dZ = E;

        for (int i = 0; i < samples; i++) {
            dZ(i, 0) /= samples;
        }

        // Weight gradient
        smat::Matrix<double> dW =
            X_poly.transpose() * dZ;

        // Bias gradient
        double dB = 0.0;

        for (int i = 0; i < samples; i++) {
            dB += dZ(i, 0);
        }

        // Parameter updates
        W = W - learning_rate * dW;

        B(0, 0) -= learning_rate * dB;

        if (epoch % 1000 == 0) {
            cout << "Epoch: " << epoch
                 << " Loss: " << loss << endl;
        }
    }

    cout << "\nFinal polynomial coefficients:\n";

    cout << "Bias: " << B(0, 0) << endl;

    for (int i = 0; i < degree; i++) {
        cout << "W" << i + 1 << ": "
             << W(i, 0) << endl;
    }

    return 0;
}
