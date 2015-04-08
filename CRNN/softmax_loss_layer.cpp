#include "softmax_loss_layer.h"
#include "utility.h"
#include "network.h"
using namespace std;

softmax_loss_layer::softmax_loss_layer(
    shared_ptr<block> input_block,
    shared_ptr<block> label_block) {
    this->m_input_block = input_block;
    this->m_label_block = label_block;
    this->m_report = false;
}

void softmax_loss_layer::setup_block() {
    if (m_input_block->size() == 0) {
        m_input_block->resize(m_label_block->size());
    }
    CHECK(m_label_block->dims().size() == 1);
    CHECK(m_input_block->size() == m_label_block->dims()[0]);
    CHECK(m_label_block->size() > 0);
};

bool softmax_loss_layer::begin_seq() {
    this->m_output_history.clear();
    return true;
}

bool softmax_loss_layer::forward(){
    auto& input = m_input_block->signal();
    arraykd &label = m_label_block->signal();
    arraykd &output = m_input_block->signal().clone();
    m_output_history.push_back(output);

    softmax_normalize(input, output);

    //compute loss
    OMP_FOR
    for (int i = 0; i < output.size(); ++i) {
        float lb = label.at(i);
        if (fabs(lb) > 1e-5) {
            m_loss_sum -= lb * log(1e-15f + output.at(i));
        }
    }
    m_loss_num += 1;

    //print output
    if (m_report) {
        for (int i = 0; i < output.size(); ++i) {
            cout << i
                << "\t" << input.at(i)
                << "\t" << output.at(i)
                << "\t" << label.at(i) << endl;
        }
        cout << "==============" << endl << endl;
    }
    return true;
}

void softmax_loss_layer::backward() {
    CHECK(!m_output_history.empty());
    auto& error = m_input_block->error();
    auto& output = m_output_history.back();
    arraykd &label = m_label_block->signal();
    m_output_history.pop_back();

    OMP_FOR
    for (int i = 0; i < error.size(); ++i) {
        error.at(i) += label.at(i) - output.at(i);
    }
}

void softmax_loss_layer::end_batch(int size) {
    m_loss_sum = 0;
    m_loss_num = 0;
}

layer_ptr create_softmax_loss_layer(
    const picojson::value& config,
    const string& layer_name,
    network *net){
    auto input_id = config.get("input").get<string>();
    auto input_block = net->block(input_id);
    auto label_block = net->block("label");
    return layer_ptr(new softmax_loss_layer(input_block, label_block));
}

REGISTER_LAYER(softmax_loss, create_softmax_loss_layer);