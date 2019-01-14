#include "tfserverclient.h"
#include <math.h>
std::__cxx11::string TFServerClient::callPredict(const std::__cxx11::string &model_name, const std::__cxx11::string &model_signature_name, QList<QList<cv::Mat> > &batch_inputs)
{
    PredictRequest predictRequest;
    PredictResponse response;
    ClientContext context;

    predictRequest.mutable_model_spec()->set_name(model_name);
    predictRequest.mutable_model_spec()->set_signature_name(
                model_signature_name);

    google::protobuf::Map<std::string, tensorflow::TensorProto>& inputs =
            *predictRequest.mutable_inputs();

    tensorflow::TensorProto proto;
    tensorflow::TensorProto proto2;

    proto.set_dtype(tensorflow::DataType::DT_FLOAT);
    proto2.set_dtype(tensorflow::DataType::DT_FLOAT);


    if(model_name == "mnet")
    {
        for(int batch = 0; batch < batch_inputs.size(); batch++)
        {
            cv::Mat m = batch_inputs.at(batch).at(0);
            cv::cvtColor(m, m,CV_BGR2RGB);
            cv::resize(m, m, cv::Size(224, 224));

            int mat_size = m.rows * m.cols * m.channels();

            for(int i = 0 ;i < mat_size; i++)
            {
                proto.add_float_val(float(m.data[i])/127.5 - 1.0);
            }

        }

        proto.mutable_tensor_shape()->add_dim()->set_size(batch_inputs.size());
        proto.mutable_tensor_shape()->add_dim()->set_size(224);
        proto.mutable_tensor_shape()->add_dim()->set_size(224);
        proto.mutable_tensor_shape()->add_dim()->set_size(3);
    }
    else if(model_name == "3daction")
    {
        for(int batch = 0; batch < batch_inputs.size(); batch++)
        {
            const QList<cv::Mat> &singal_batch = batch_inputs.at(batch);

            if(singal_batch.size()!=16)
            {
                std::cout << "wrong time_step size of 3d action" << std::endl;
            }

            for(int time_step = 0; time_step < 16 ; time_step ++)
            {
                cv::Mat m = singal_batch.at(time_step);
                cv::cvtColor(m,m,CV_BGR2RGB);

                float height = m.rows;
                float width = m.cols;

                if(width > height)
                {
                    float scale = 160.0/height;
                    cv::resize(m, m, cv::Size(int(width*scale+1), 160));
                }
                else
                {
                    float scale = 160.0/width;
                    cv::resize(m, m, cv::Size(160, int(height*scale+1)));
                }


                int crop_y = (m.cols-160)/2;
                int crop_x = (m.rows-160)/2;
                cv::Rect roi(crop_x, crop_y, 160, 160);
                m = m(roi);

                cv::Mat tmp_mean, tmp_std;
                cv::meanStdDev(m, tmp_mean, tmp_std);
                float mean_value = tmp_mean.at<double>(0, 0);
                float std_value = MAX(tmp_std.at<double>(0, 0), 1.0/sqrt(160.0*160.0*3.0));

                int mat_size = m.rows * m.cols * m.channels();

                for(int i =0 ;i< mat_size; i++)
                {
                    proto.add_float_val((float(m.data[i])-mean_value)/std_value );
                }
            }

        }

        proto.mutable_tensor_shape()->add_dim()->set_size(batch_inputs.size());
        proto.mutable_tensor_shape()->add_dim()->set_size(16);
        proto.mutable_tensor_shape()->add_dim()->set_size(160);
        proto.mutable_tensor_shape()->add_dim()->set_size(160);
        proto.mutable_tensor_shape()->add_dim()->set_size(3);
    }
    else if(model_name == "yolov3")
    {
        cv::Mat m = batch_inputs.at(0).at(0);
        cv::cvtColor(m,m,CV_BGR2RGB);

        float shape0 = m.rows;
        float shape1= m.cols;
        proto2.add_float_val(shape0);
        proto2.add_float_val(shape1);
        proto2.mutable_tensor_shape()->add_dim()->set_size(2);
        inputs["shape"] = proto2;

        float scale = MIN(416.0/shape0, 416.0/shape1);
        int new_w = shape1*scale;
        int new_h = shape0*scale;

        cv::resize(m, m, cv::Size(new_w, new_h),0,0, cv::INTER_CUBIC);
        cv::copyMakeBorder(m, m, (416-new_h)/2, 416-new_h-(416-new_h)/2, (416-new_w)/2, 416-new_w-(416-new_w)/2, cv::BORDER_CONSTANT, cv::Scalar(128));

        int mat_size = m.rows * m.cols * m.channels();

        for(int i =0 ;i< mat_size; i++)
        {
            proto.add_float_val(float(m.data[i])/255.0);
        }

        proto.mutable_tensor_shape()->add_dim()->set_size(1);
        proto.mutable_tensor_shape()->add_dim()->set_size(416);
        proto.mutable_tensor_shape()->add_dim()->set_size(416);
        proto.mutable_tensor_shape()->add_dim()->set_size(3);

    }
    else
    {

    }

    inputs["x"] = proto;

    Status status = stub_->Predict(&context, predictRequest, &response);

    if (status.ok()) {
        std::cout << "call predict ok" << std::endl;
        std::cout << "outputs size is " << response.outputs_size() << std::endl;
        OutMap& map_outputs = *response.mutable_outputs();

        if(model_name == "mnet")
        {
            tensorflow::TensorProto& result_tensor_proto = map_outputs["scores"];
            std::cout << "scores" << std::endl;
            for (int i = 0; i < result_tensor_proto.float_val_size(); ++i) {
                float val = result_tensor_proto.float_val(i);
                std::cout << "probability of " << i%(result_tensor_proto.float_val_size() / batch_inputs.size()) << " is " << val << std::endl;

            }
        }
        else if(model_name == "3daction")
        {
            tensorflow::TensorProto& result_tensor_proto = map_outputs["logit"];
            std::cout << "3daction" << std::endl;
            for (int i = 0; i < result_tensor_proto.float_val_size(); ++i) {
                float val = result_tensor_proto.float_val(i);
                std::cout << "probability of " << i%(result_tensor_proto.float_val_size() / batch_inputs.size()) << " is " << val << std::endl;

            }
        }
        else if(model_name == "yolov3")
        {
            tensorflow::TensorProto& result_tensor_proto_b = map_outputs["boxes"];
            std::cout << "boxes" << std::endl;
            for (int i = 0; i < result_tensor_proto_b.float_val_size(); ++i) {
                float val = result_tensor_proto_b.float_val(i);
                std::cout << "probability of " << i%4 << " is " << val << std::endl;

            }

            tensorflow::TensorProto& result_tensor_proto_s = map_outputs["scores"];
            std::cout << "scores" << std::endl;
            for (int i = 0; i < result_tensor_proto_s.float_val_size(); ++i) {
                float val = result_tensor_proto_s.float_val(i);
                std::cout << "probability of " << i << " is " << val << std::endl;

            }

            tensorflow::TensorProto& result_tensor_proto_c = map_outputs["classes"];
            std::cout << "classes" << std::endl;
            for (int i = 0; i < result_tensor_proto_c.int_val_size(); ++i) {
                int val = result_tensor_proto_c.int_val(i);
                std::cout << "probability of " << i << " is " << val << std::endl;

            }
        }
        else
        {

        }

        return "Done.";
    } else {
        std::cout << "gRPC call return code: " << status.error_code() << ": "
                  << status.error_message() << std::endl;
        return "gRPC failed.";
    }
}
