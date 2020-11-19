#include <iostream>
#include <string>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <unistd.h>


int main()
{
    std::string topic = "VisualEvent";
    zmq::context_t context(1);
    zmq::socket_t subscriber(context, ZMQ_SUB);
    subscriber.connect("tcp://127.0.0.1:6543");
    subscriber.set(zmq::sockopt::subscribe, topic);
    std::cout << "Connected. "<< std::endl;
    nlohmann::json vocs;
    sleep(1);

    while(true)
    {
        std::cout << "Waiting for message" << std::endl;
        zmq::message_t msg;

        //Fill a message passed by reference
        auto res = subscriber.recv(msg, zmq::recv_flags::none);
        std::string s = std::string(static_cast<char*>(msg.data()), msg.size());
        if( s == topic) {
            zmq::message_t json_msg;
            subscriber.recv(&json_msg);
            std::string json_str = std::string(static_cast<char*>(json_msg.data()), json_msg.size());
            vocs = nlohmann::json::parse(json_str);
            if (vocs.size() > 0) {
                std::cout << vocs << std::endl;
                std::cout << vocs.size() << std::endl;
                nlohmann::json itvoc;
                for (int i=0; i < vocs.size(); i++) {
                    if (vocs[i].is_object()) {
                        std::cout << vocs[i]["xmin"] << std::endl;
                    }
                }
            }
            sleep(3);
        }
    }
    return 0;
}
