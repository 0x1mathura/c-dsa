
#include "lib/SimpleMatrix.h"
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

using namespace std;

void weightInitialization(smat::Matrix<double>& W,int rows, int cols) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            W(i, j) = (rand() % 1000) / 1000.0 - 0.5;
        }


    }
}

void biasInitialization(smat::Matrix<double>& B, int rows, int cols) {

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            B(i, j) = 0.0;
        }
    }
}

double sigmoid(double z) {
    return 1.0 / (1.0 + exp(-z));
}

void addBias(smat::Matrix<double>& Z,const smat::Matrix<double>& B,int samples, int neurons) {

    for (int i = 0; i < samples; i++) {
        for (int j = 0; j < neurons; j++) {
            Z(i, j) += B(0, j);
        }
    }
}

smat::Matrix<double> sigmoidMatrix(const smat::Matrix<double>& Z,int rows, int cols) {

    smat::Matrix<double> A(rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            A(i, j) = sigmoid(Z(i, j));
        }
    }

    return A;
}

smat::Matrix<double> sigmoidBackward(const smat::Matrix<double>& dA,const smat::Matrix<double>& A,int rows, int cols) {

    smat::Matrix<double> dZ(rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            dZ(i, j) = dA(i, j) * A(i, j) * (1.0 - A(i, j));
        }
    }

    return dZ;
}

smat::Matrix<double> biasGradient(const smat::Matrix<double>& dZ, int samples, int neurons) {

    smat::Matrix<double> dB(1, neurons);

    for (int j = 0; j < neurons; j++) {

        dB(0, j) = 0.0;

        for (int i = 0; i < samples; i++) {
            dB(0, j) += dZ(i, j);
        }
    }

    return dB;
}

int main() {

    const int samples = 4;
    const int inputSize = 2;
    const int hidden1 = 3;
    const int hidden2 = 3;
    const int outputSize = 1;


    // XOR dataset
    smat::Matrix<double> X(4, 2);
    smat::Matrix<double> Y(4, 1);

    double data[4][2] = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}
    };

    double labels[4] = {0, 1, 1, 0};

    for (int i = 0; i < samples; i++) {

        for (int j = 0; j < inputSize; j++) {
            X(i, j) = data[i][j];
        }

        Y(i, 0) = labels[i];
    }

    // Network parameters
    smat::Matrix<double> W1(inputSize, hidden1);
    smat::Matrix<double> B1(1, hidden1);

    smat::Matrix<double> W2(hidden1, hidden2);
    smat::Matrix<double> B2(1, hidden2);

    smat::Matrix<double> W3(hidden2, outputSize);
    smat::Matrix<double> B3(1, outputSize);

    weightInitialization(W1, inputSize, hidden1);
    weightInitialization(W2, hidden1, hidden2);
    weightInitialization(W3, hidden2, outputSize);

    biasInitialization(B1, 1, hidden1);
    biasInitialization(B2, 1, hidden2);
    biasInitialization(B3, 1, outputSize);

    double learning_rate = 0.5;
    int epochs = 30000;

    for (int epoch = 0; epoch < epochs; epoch++) {

        // FORWARD PROPAGATION
        smat::Matrix<double> Z1 = X * W1;
        addBias(Z1, B1, samples, hidden1);

        smat::Matrix<double> A1 = sigmoidMatrix(Z1, samples, hidden1);

        smat::Matrix<double> Z2 = A1 * W2;
        addBias(Z2, B2, samples, hidden2);

        smat::Matrix<double> A2 = sigmoidMatrix(Z2, samples, hidden2);

        smat::Matrix<double> Z3 = A2 * W3;
        addBias(Z3, B3, samples, outputSize);

        smat::Matrix<double> A3 = sigmoidMatrix(Z3, samples, outputSize);


        double loss = 0.0;

        for (int i = 0; i < samples; i++) {

            double p = A3(i, 0);

            p = max(1e-15, min(1.0 - 1e-15, p));

            loss -= Y(i, 0) * log(p) + (1.0 - Y(i, 0)) * log(1.0 - p);
        }

        loss /= samples;


        // BACKPROPAGATION

        // Output layer
        smat::Matrix<double> dZ3 = A3 - Y;

        for (int i = 0; i < samples; i++) {
            dZ3(i, 0) /= samples;
        }

        smat::Matrix<double> dW3 = A2.transpose() * dZ3;

        smat::Matrix<double> dB3 = biasGradient(dZ3, samples, outputSize);

        smat::Matrix<double> dA2 = dZ3 * W3.transpose();

        smat::Matrix<double> dZ2 = sigmoidBackward(dA2, A2, samples, hidden2);

        smat::Matrix<double> dW2 = A1.transpose() * dZ2;

        smat::Matrix<double> dB2 = biasGradient(dZ2, samples, hidden2);

        // Hidden layer 1
        smat::Matrix<double> dA1 = dZ2 * W2.transpose();

        smat::Matrix<double> dZ1 = sigmoidBackward(dA1, A1, samples, hidden1);

        smat::Matrix<double> dW1 = X.transpose() * dZ1;

        smat::Matrix<double> dB1 = biasGradient(dZ1, samples, hidden1);


        // UPDATE PARAMETERS
        W1 = W1 - learning_rate * dW1;
        B1 = B1 - learning_rate * dB1;

        W2 = W2 - learning_rate * dW2;
        B2 = B2 - learning_rate * dB2;

        W3 = W3 - learning_rate * dW3;
        B3 = B3 - learning_rate * dB3;

        if (epoch % 3000 == 0) {
            cout << "Epoch: " << epoch
                 << " Loss: " << loss << endl;
        }
    }


    // FINAL PREDICTIONS

    smat::Matrix<double> Z1 = X * W1;
    addBias(Z1, B1, samples, hidden1);

    smat::Matrix<double> A1 = sigmoidMatrix(Z1, samples, hidden1);

    smat::Matrix<double> Z2 = A1 * W2;
    addBias(Z2, B2, samples, hidden2);

    smat::Matrix<double> A2 = sigmoidMatrix(Z2, samples, hidden2);

    smat::Matrix<double> Z3 = A2 * W3;
    addBias(Z3, B3, samples, outputSize);

    smat::Matrix<double> predictions = sigmoidMatrix(Z3, samples, outputSize);

    cout << "\nFinal XOR predictions:\n";

    for (int i = 0; i < samples; i++) {

        double probability = predictions(i, 0);

        int predictedClass = probability >= 0.5 ? 1 : 0;

        cout << X(i, 0) << " XOR "
             << X(i, 1)
             << " = " << probability
             << " -> " << predictedClass << endl;
    }

    return 0;
}
