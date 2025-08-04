#include "PCA_AnomalyDetector.hpp"

int main() 
{
    MatrixXd X(6, 3);
    X << 2.5, 2.4, 0.5,
         0.5, 0.7, 0.2,
         2.2, 2.9, 0.3,
         1.9, 2.2, 0.4,
         10.0, 10.1, 5.0,
         11.0, 10.9, 4.5;  // anomaly

    VectorXi y_true(6);
    VectorXi labels;
    y_true << 0, 0, 0, 0, 1, 1;

    PCA pca_method;
    pca_method.Fit(X);
    labels = pca_method.Predict(X);

    pca_method.Evaluate(y_true, labels);
}
