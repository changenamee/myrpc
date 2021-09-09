#include"include/rpcController.h"

rpcController::rpcController():failed(false),m_errtext(""){

}

void rpcController::ReSet(){
    failed = false;
    m_errtext = "";
}

bool rpcController::Failed() const{
    return failed;
}

std::string rpcController::ErrText() const{
    return m_errtext;
}

void rpcController::SetFailed(const std::string& errmsg){
    m_errtext = errmsg;
}
