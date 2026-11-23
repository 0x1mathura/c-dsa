#include <iostream>
#include <cstdlib>
#include <cmath>
#include "lib/SimpleMatrix.h"

using namespace std;

void weightInitialization( smat::Matrix<double>& weights, int rows, int cols) {

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                weights(i, j) = (rand() % 100) / 100.0;
            }
        }
}

void biasInitialization( smat::Matrix<double>& bias, int rows, int cols) {

        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                bias(i, j) = 0.0;
            }
        }
}

double sigmoid(double z) {
    return 1.0 / (1.0 + exp(-z));
}

int main() {

    // 4 samples, 3 features
    smat::Matrix<double> X(4, 3);

    // Binary labels
    smat::Matrix<double> Y(4, 1);

    // 3 features, 1 output
    smat::Matrix<double> W(3, 1);

    // One bias
    smat::Matrix<double> B(1, 1);

    // Training data
    double data[4][3] = {
        {1, 2, 3},
        {2, 3, 4},
        {3, 4, 5},
        {4, 5, 6}
    };

    // Binary classification labels
    double labels[4] = {0, 0, 1, 1};

    for (int i = 0; i < 4; i++) {

        for (int j = 0; j < 3; j++) {
            X(i, j) = data[i][j];
        }

        Y(i, 0) = labels[i];
    }

    weightInitialization(W, 3, 1);
    biasInitialization(B, 1, 1);

    double learning_rate = 0.01;
    int epochs = 2000;

    int m = 4;

    for (int epoch = 0; epoch < epochs; epoch++) {

        // Forward propagation
        smat::Matrix<double> Z = X * W;

        // Add bias
        for (int i = 0; i < m; i++) {
            Z(i, 0) += B(0, 0);
        }

        // Sigmoid activation
        smat::Matrix<double> Y_hat(m, 1);

        for (int i = 0; i < m; i++) {
            Y_hat(i, 0) = sigmoid(Z(i, 0));
        }

        // Binary cross entropy
        double loss = 0.0;

        for (int i = 0; i < m; i++) {

            double p = Y_hat(i, 0);


            loss -= Y(i, 0) * log(p) + (1.0 - Y(i, 0)) * log(1.0 - p);
        }

        loss /= m;

        // Error derivative: dZ = (Y_hat - Y) / m
        smat::Matrix<double> dZ = Y_hat - Y;

        for (int i = 0; i < m; i++) {
            dZ(i, 0) /= m;
        }

        // Weight gradient
        smat::Matrix<double> dW = X.transpose() * dZ;

        // Bias gradient
        double dB = 0.0;

        for (int i = 0; i < m; i++) {
            dB += dZ(i, 0);
        }

        // Update parameters
        W = W - learning_rate * dW;

        B(0, 0) -= learning_rate * dB;

        if (epoch % 100 == 0) {
            cout << "Epoch: " << epoch
                 << " Loss: " << loss << endl;
        }
    }

    cout << "\nFinal weights:\n";

    for (int i = 0; i < 3; i++) {
        cout << W(i, 0) << endl;
    }

    cout << "\nFinal bias: " << B(0, 0) << endl;

    // Predictions
    cout << "\nPredictions:\n";

    smat::Matrix<double> Z = X * W;

    for (int i = 0; i < m; i++) {

        Z(i, 0) += B(0, 0);

        double probability = sigmoid(Z(i, 0));

        int prediction = probability >= 0.5 ? 1 : 0;

        cout << "Probability: " << probability
             << " Class: " << prediction << endl;
    }

    return 0;
}
