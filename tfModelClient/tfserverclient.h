#ifndef TFSERVERCLIENT_H
#define TFSERVERCLIENT_H

#include <fstream>
#include <iostream>

#include "grpcpp/create_channel.h"
#include "grpcpp/security/credentials.h"
#include "tensor.grpc.pb.h"
#include "prediction_service.grpc.pb.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <QVector>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using tensorflow::serving::PredictRequest;
using tensorflow::serving::PredictResponse;
using tensorflow::serving::PredictionService;

typedef google::protobuf::Map<std::string, tensorflow::TensorProto> OutMap;

class TFServerClient {
public:
    TFServerClient(std::shared_ptr<Channel> channel)
        : stub_(PredictionService::NewStub(channel)) {}

    bool callPredict(const std::string& model_name,
                            const std::string& model_signature_name,
                            QVector<QVector<cv::Mat> >& batch_inputs,
                            QVector<QVector<float> > &predict_outputs);

private:
    std::unique_ptr<PredictionService::Stub> stub_;
};

#endif // TFSERVERCLIENT_H
