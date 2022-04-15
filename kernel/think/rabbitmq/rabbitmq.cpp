#include "kernel/think/rabbitmq/rabbitmq.hpp"
#include "kernel/common/logger/logger.hpp"
#include "kernel/common/tools/tools.hpp"
#include <iostream>

//ERROR_CODE think::Crabbitmq::XXXXXX(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
//    //check is error happened at last STEP
//    if(CErrorCode::isErrorHappened(ret)){
//        return ret;
//    }
//
//    try{
//        return ERR_THINK_XXXXXX_BLOCK;
//    } catch (std::exception& e){
//        throw;
//        return ERR_THINK_XXXXXX_EXCEPTION;
//    } catch (...){
//        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
//        throw;
//        return ERR_THINK_XXXXXX_EXCEPTION;
//    }
//
//    return ERR_OK;
//}

ERROR_CODE think::Crabbitmq::ErrorWrapper(amqp_rpc_reply_t x, char const *context){
    switch (x.reply_type) {
        case AMQP_RESPONSE_NORMAL:
        return ERR_OK;

        case AMQP_RESPONSE_NONE:
        fprintf(stderr, "%s: missing RPC reply type!\n", context);
        break;

        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
        fprintf(stderr, "%s: %s\n", context, amqp_error_string2(x.library_error));
        break;

        case AMQP_RESPONSE_SERVER_EXCEPTION:
        switch (x.reply.id) {
            case AMQP_CONNECTION_CLOSE_METHOD: {
            amqp_connection_close_t *m = (amqp_connection_close_t *)x.reply.decoded;
            fprintf(stderr, "%s: server connection error %uh, message: %.*s\n",
                    context, m->reply_code, (int)m->reply_text.len,
                    (char *)m->reply_text.bytes);
            break;
            }
            case AMQP_CHANNEL_CLOSE_METHOD: {
            amqp_channel_close_t *m = (amqp_channel_close_t *)x.reply.decoded;
            fprintf(stderr, "%s: server channel error %uh, message: %.*s\n",
                    context, m->reply_code, (int)m->reply_text.len,
                    (char *)m->reply_text.bytes);
            break;
            }
            default:
            fprintf(stderr, "%s: unknown server error, method id 0x%08X\n",
                    context, x.reply.id);
            break;
        }
        break;
    }
    return ERR_THINK_RABBITMQ_BLOCK;
}

std::vector<std::string> think::Crabbitmq::Connection_SplitStr(std::string str,std::string pattern){ 
    std::string::size_type pos;  
    std::vector<std::string> result; 
    str+=pattern;//扩展字符串以方便操作  
    int size=str.size();    
    for(int i=0; i<size; i++)  
    {    
        pos=str.find(pattern,i);    
        if(pos<size)    
        {     
            std::string s=str.substr(i,pos-i);      
            result.push_back(s);     
            i=pos+pattern.size()-1;    
        }  
    }  
    return result;
}

ERROR_CODE think::Crabbitmq::Connection(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        moduleParam->channel = (moduleParam->channel==0)?1:moduleParam->channel;
        int heartbeat = (moduleParam->heartbeat<=0)?0:moduleParam->heartbeat;
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel;
        amqp_connection_state_t connection;

        amqp_socket_t *socket = NULL;

        connection = amqp_new_connection();

        socket = amqp_tcp_socket_new(connection);
        if (!socket) {
            return ERR_THINK_RABBITMQ_CREATECONNECT_BLOCK;
        }

        // const char* hostname = CONFIG_RABBITMQHOST;
        // int port = CONFIG_RABBITMQPORT;
        // int status = amqp_socket_open(socket, hostname, port);
        // if (status) {
        //     return ERR_THINK_RABBITMQ_OPENCONNECT_BLOCK;
        // }

        int status = 0;
        struct timeval timeout;
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        std::string address = moduleParam->connectAdress;
        std::cout << "link RabbitMQ:" << address << std::endl;
        std::vector<std::string> connectList = Connection_SplitStr(address,",");
        for(int i=0;i<connectList.size();i++){
            std::vector<std::string> tempConnect = Connection_SplitStr(connectList[i],":");
            //status = amqp_socket_open(socket, tempConnect[0].c_str(), atoi(tempConnect[1].c_str()));
            status = amqp_socket_open_noblock(socket, tempConnect[0].c_str(), atoi(tempConnect[1].c_str()), &timeout);
            if (status) {
                //return ERR_THINK_RABBITMQ_OPENCONNECT_BLOCK;
            }else{
                break;
            }
        }
        if(status || connectList.size()==0){
            ERRORLOGGER("Can not link RabbitMQ: "+address);
            return ERR_THINK_RABBITMQ_OPENCONNECT_BLOCK;
        }

        ret = this->ErrorWrapper(amqp_login(connection, "/", 0, 131072, heartbeat, AMQP_SASL_METHOD_PLAIN, moduleParam->connectUser.c_str(), moduleParam->connectPwd.c_str()),"Logging in");
        if(CErrorCode::isErrorHappened(ret)){
            return ERR_THINK_RABBITMQ_LOGGIN_BLOCK;
        }

        amqp_channel_open(connection, channel);
        moduleParam->connection = connection;

        ret = this->ErrorWrapper(amqp_get_rpc_reply(connection), "Opening channel");
        if(CErrorCode::isErrorHappened(ret)){
            return ERR_THINK_RABBITMQ_CHANNELOPEN_BLOCK;
        }

    } catch (std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_CONNECT_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_CONNECT_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::DisConnection(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel;
        amqp_connection_state_t connection = moduleParam->connection;

        ret = this->ErrorWrapper(amqp_channel_close(connection, channel, AMQP_REPLY_SUCCESS), "Closing channel");
        if(CErrorCode::isErrorHappened(ret)){
            ERRORLOGGER("closing channel error");
            //return ERR_THINK_RABBITMQ_CHANNELCLOSE_BLOCK;
        }

        ret = this->ErrorWrapper(amqp_connection_close(connection, AMQP_REPLY_SUCCESS), "Closing connection");
        if(CErrorCode::isErrorHappened(ret)){
            ERRORLOGGER("closing connect error");
            //return ERR_THINK_RABBITMQ_CONNECTCLOSE_BLOCK;
        }
        if(amqp_destroy_connection(connection)<0){
            return ERR_THINK_RABBITMQ_CONNECTDESTROY_BLOCK;
        }
    } catch (std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_CONNECTCLOSE_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_CONNECTCLOSE_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::DeclareExchange(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel;
        const char* exchange = moduleParam->exchange;
        const char* exchangetype = moduleParam->exchangetype?moduleParam->exchangetype:"direct";
        amqp_connection_state_t connection = moduleParam->connection;
        if(!exchange){
            return ret;
        }

        amqp_exchange_declare(connection, channel, amqp_cstring_bytes(exchange), amqp_cstring_bytes(exchangetype), 0, 0, 0, 0, amqp_empty_table);
        ret = this->ErrorWrapper(amqp_get_rpc_reply(connection), "Declaring exchange");
        if(CErrorCode::isErrorHappened(ret)){
            return ERR_THINK_RABBITMQ_EXCHANGEDECLARE_BLOCK;
        }
        
    } catch (std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_EXCHANGEDECLARE_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_EXCHANGEDECLARE_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::DeclareQueue(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        amqp_bytes_t queuename;
        amqp_boolean_t durable = (amqp_boolean_t)moduleParam->durable;     //永久保存
        amqp_boolean_t autodelete = (amqp_boolean_t)moduleParam->autodelete;
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel;
        const char* exchange = moduleParam->exchange;
        const char* bindingkey = moduleParam->bindingkey;
        const char* queuenamestr = moduleParam->queuenamestr;
        amqp_connection_state_t connection = moduleParam->connection;

        amqp_queue_declare_ok_t *r;
        if(queuenamestr){
            r = amqp_queue_declare(connection, channel, amqp_cstring_bytes(queuenamestr), 0, durable, 0, autodelete, amqp_empty_table);
        } else {
            r = amqp_queue_declare(connection, channel, amqp_empty_bytes, 0, durable, 0, autodelete, amqp_empty_table);
        }
        ret = this->ErrorWrapper(amqp_get_rpc_reply(connection), "Declaring queue");
        if(CErrorCode::isErrorHappened(ret)){
            return ERR_THINK_RABBITMQ_QUEUEDECLARE_BLOCK;
        }

        queuename = amqp_bytes_malloc_dup(r->queue);
        if (queuename.bytes == NULL) {
            return ERR_THINK_RABBITMQ_QUEUEDECLARENAME_BLOCK;
        }

        if(exchange && strcmp(exchange, "")!=0){
            if(bindingkey){
                amqp_queue_bind(connection, channel, queuename, amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey), amqp_empty_table);
            } else {
                amqp_queue_bind(connection, channel, queuename, amqp_cstring_bytes(exchange), amqp_cstring_bytes(""), amqp_empty_table);
            }
            ret = this->ErrorWrapper(amqp_get_rpc_reply(connection), "Binding queue");
            if(CErrorCode::isErrorHappened(ret)){
                return ERR_THINK_RABBITMQ_QUEUEBLIND_BLOCK;
            } else {
                moduleParam->queuename = queuename;
            }
        } else {
            moduleParam->queuename = queuename;
        }
        
    } catch (std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_QUEUEDECLARE_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_QUEUEDECLARE_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::DeclareRPCQueue(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        amqp_bytes_t queuename;
        amqp_boolean_t durable = 0;     //永久保存
        amqp_boolean_t autodelete = 1;
        const char* queuenamestr = moduleParam->queuenamerpcstr;
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel;
        amqp_connection_state_t connection = moduleParam->connection;

        amqp_queue_declare_ok_t *r;
        if(queuenamestr) {
            r = amqp_queue_declare(connection, channel, amqp_cstring_bytes(queuenamestr), 0, durable, 0, autodelete, amqp_empty_table);
        } else {
            r = amqp_queue_declare(connection, channel, amqp_empty_bytes, 0, durable, 0, autodelete, amqp_empty_table);
        }
        ret = this->ErrorWrapper(amqp_get_rpc_reply(connection), "Declaring RPC queue");
        if(CErrorCode::isErrorHappened(ret)){
            return ERR_THINK_RABBITMQ_QUEUEDECLARE_BLOCK;
        }

        queuename = amqp_bytes_malloc_dup(r->queue);
        if (queuename.bytes == NULL) {
            return ERR_THINK_RABBITMQ_QUEUEDECLARENAME_BLOCK;
        }

        moduleParam->queuenamerpc = queuename;      
        
    } catch (std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_QUEUEDECLARE_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_QUEUEDECLARE_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::UnbindQueue(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        const char* exchange = moduleParam->exchange;
        const char* bindingkey = moduleParam->bindingkey;
        const char* queuenamestr = moduleParam->queuenamestr;
        amqp_bytes_t queuename = moduleParam->queuename;
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel;
        amqp_connection_state_t connection = moduleParam->connection;

        if(queuenamestr){
            amqp_queue_unbind(connection, channel, amqp_cstring_bytes(queuenamestr), amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey), amqp_empty_table);
        } else {
            amqp_queue_unbind(connection, channel, queuename, amqp_cstring_bytes(exchange), amqp_cstring_bytes(bindingkey), amqp_empty_table);
        }
        ret = this->ErrorWrapper(amqp_get_rpc_reply(connection), "Declaring exchange");
        if(CErrorCode::isErrorHappened(ret)){
            return ERR_THINK_RABBITMQ_UNBINDQUEUE_BLOCK;
        }
        
    } catch (std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_UNBINDQUEUE_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_UNBINDQUEUE_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::QosPrefetch(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        uint16_t prefetch_count = 1;
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel;
        amqp_connection_state_t connection = moduleParam->connection;
        
        if (!amqp_basic_qos(connection, channel, 0, prefetch_count, 0)) {
            return ERR_THINK_RABBITMQ_QOSPREFETCH_BLOCK;
        }
    } catch(std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_QOSPREFETCH_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_QOSPREFETCH_EXCEPTION;
    }

    return ERR_OK;       

}

ERROR_CODE think::Crabbitmq::GetMessage(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        amqp_bytes_t queuename = moduleParam->queuename;
        const char *returnkey = moduleParam->returnkey;
        amqp_boolean_t noack = (amqp_boolean_t)moduleParam->noack;
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel;
        amqp_connection_state_t connection = moduleParam->connection;

        amqp_basic_consume(connection, channel, queuename, amqp_empty_bytes, 0, noack, 0,amqp_empty_table);
        ret = this->ErrorWrapper(amqp_get_rpc_reply(connection), "Consuming");
        if(CErrorCode::isErrorHappened(ret)){
            return ERR_THINK_RABBITMQ_CONSUME_BLOCK;
        }


        //STEP get message
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(connection);

        res = amqp_consume_message(connection, &envelope, NULL, 0);

        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            return ERR_THINK_RABBITMQ_GETMESSAGE_BLOCK;
        }

        moduleParam->deliverytag = envelope.delivery_tag;
        
        if (envelope.message.properties._flags & AMQP_BASIC_REPLY_TO_FLAG) {
            moduleParam->queuenamerpc = amqp_bytes_malloc_dup(envelope.message.properties.reply_to);
        }

        Json::Reader reader;
        Json::Value tempJson;
        reader.parse((const char *)envelope.message.body.bytes,((const char *)envelope.message.body.bytes + envelope.message.body.len), tempJson);
        if(returnkey){
            passParam[returnkey] = tempJson;
        }else{
            if(!returnParam.empty()){
                returnParam.clear();
            }
            returnParam = tempJson;
        }

        if (envelope.message.properties._flags & AMQP_BASIC_CORRELATION_ID_FLAG) {
            moduleParam->correlationidrpc = amqp_bytes_malloc_dup(envelope.message.properties.correlation_id);
            char tempCorrelationId[2048];
            sprintf(tempCorrelationId, "%.*s", (int)envelope.message.properties.correlation_id.len, (char *)envelope.message.properties.correlation_id.bytes);
            std::cout << tempCorrelationId<< std::endl;
            moduleParam->correlationidget = tempCorrelationId;
        }else if(returnParam.isMember("missionId")){
            moduleParam->correlationidrpc = amqp_cstring_bytes(tempJson["missionId"].asString().c_str());
            moduleParam->correlationidget = tempJson["missionId"].asString();
        } else {
            moduleParam->correlationidrpc = amqp_cstring_bytes("unknown");
            moduleParam->correlationidget = "unknown";
        }
        
        amqp_destroy_envelope(&envelope);
    } catch(std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_GETMESSAGE_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_GETMESSAGE_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::GetResponseMessage(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        amqp_bytes_t queuename = moduleParam->queuenamerpc;
        const char *returnkey = moduleParam->returnkey;
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel;
        amqp_boolean_t noack = 1;
        amqp_connection_state_t connection = moduleParam->connection;
        int isrpc = moduleParam->isrpc;

        amqp_basic_consume(connection, channel, queuename, amqp_empty_bytes, 0, noack, 0,amqp_empty_table);
        ret = this->ErrorWrapper(amqp_get_rpc_reply(connection), "Consuming");
        if(CErrorCode::isErrorHappened(ret)){
            return ERR_THINK_RABBITMQ_CONSUME_BLOCK;
        }


        //STEP get message
        amqp_rpc_reply_t res;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(connection);

        res = amqp_consume_message(connection, &envelope, NULL, 0);

        if (AMQP_RESPONSE_NORMAL != res.reply_type) {
            return ERR_THINK_RABBITMQ_GETMESSAGE_BLOCK;
        }

        moduleParam->deliverytag = envelope.delivery_tag;
        
        printf("**Content-type: %.*s\n",
             (int)envelope.message.properties.content_type.len,
             (char *)envelope.message.properties.content_type.bytes);
        printf("**context:%.*s\n",
            envelope.message.body.len,
            (const char *)envelope.message.body.bytes);
        
        if (envelope.message.properties._flags & AMQP_BASIC_REPLY_TO_FLAG) {
            moduleParam->queuenamerpc = amqp_bytes_malloc_dup(envelope.message.properties.reply_to);
        }

        Json::Reader reader;
        Json::Value tempJson;
        reader.parse((const char *)envelope.message.body.bytes,((const char *)envelope.message.body.bytes + envelope.message.body.len), tempJson);
        if(returnkey){
            passParam[returnkey] = tempJson;
        }else{
            passParam.clear();
            passParam = tempJson;
        }

        if (envelope.message.properties._flags & AMQP_BASIC_CORRELATION_ID_FLAG) {
            moduleParam->correlationidrpc = amqp_bytes_malloc_dup(envelope.message.properties.correlation_id);
            char tempCorrelationId[2048];
            sprintf(tempCorrelationId, "%.*s", (int)envelope.message.properties.correlation_id.len, (char *)envelope.message.properties.correlation_id.bytes);
            std::cout << tempCorrelationId<< std::endl;
            moduleParam->correlationidget = tempCorrelationId;
        } else if(returnParam.isMember("missionId")){
            moduleParam->correlationidrpc = amqp_cstring_bytes(tempJson["missionId"].asString().c_str());
            moduleParam->correlationidget = tempJson["missionId"].asString();
        } else {
            moduleParam->correlationidrpc = amqp_cstring_bytes("unknown");
            moduleParam->correlationidget = "unknown";
        }

        amqp_destroy_envelope(&envelope);
        if(isrpc){
            amqp_bytes_free(moduleParam->queuenamerpc);
        }
    } catch(std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_GETMESSAGE_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_GETMESSAGE_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::MessageACK(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel; 
        amqp_connection_state_t connection = moduleParam->connection;

        if (!amqp_basic_ack(connection, channel, moduleParam->deliverytag, 0)) {
            LOGGER("missionget@mission ack error!!!");
            return ERR_THINK_RABBITMQ_MESSAGEACK_BLOCK;
        }
        
    } catch (std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_MESSAGEACK_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_MESSAGEACK_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::SendMessage(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        moduleParam->sendStr = moduleParam->sendStr?moduleParam->sendStr:"sendStr";
        const char* exchange = moduleParam->exchange;
        const char* routingkey = moduleParam->routingkey;
        const char* correlationid = (moduleParam->correlationid)?moduleParam->correlationid:"";
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel; 
        int isrpc = moduleParam->isrpc;
        amqp_bytes_t queuenamerpc = moduleParam->queuenamerpc;
        if(!passParam.isMember(moduleParam->sendStr)){
            return ERR_THINK_RABBITMQ_SENDMESSAGEEMPTY_BLOCK;
        }
        Json::FastWriter swriter;
        Json::Value sendJson = passParam[moduleParam->sendStr];
        std::string messageStr = swriter.write(sendJson);
        const char* messagebody = messageStr.c_str();
        printf("***sending:%s\n", messagebody);
        amqp_basic_properties_t props;
        amqp_connection_state_t connection = moduleParam->connection;

        if(isrpc){
            props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | 
                            AMQP_BASIC_REPLY_TO_FLAG | AMQP_BASIC_CORRELATION_ID_FLAG;
        } else {
            props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_CORRELATION_ID_FLAG;
        }

        props.content_type = amqp_cstring_bytes("text/plain");
        props.delivery_mode = 2; /* persistent delivery mode */
        if(isrpc){
            props.reply_to = queuenamerpc;
        }

        props.correlation_id = amqp_cstring_bytes(correlationid);

        int tmpRet = amqp_basic_publish(connection, channel, amqp_cstring_bytes(exchange),amqp_cstring_bytes(routingkey), 0, 0, &props, amqp_cstring_bytes(messagebody));
        if(tmpRet<0){
            return ERR_THINK_RABBITMQ_SENDMESSAGE_BLOCK;
        }

    } catch (std::exception& e){
        std::cout<<e.what()<<std::endl;
        throw;
        return ERR_THINK_RABBITMQ_SENDMESSAGE_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_SENDMESSAGE_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::SendResponseMessage(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        moduleParam->sendStr = moduleParam->sendStr?moduleParam->sendStr:"sendStr";
        const char* exchange = "";
        amqp_bytes_t routingkey = moduleParam->queuenamerpc;
        amqp_bytes_t correlationid = moduleParam->correlationidrpc;
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel; 
        amqp_connection_state_t connection = moduleParam->connection;
        if(!passParam.isMember(moduleParam->sendStr)){
            return ERR_THINK_RABBITMQ_SENDMESSAGEEMPTY_BLOCK;
        }

        Json::FastWriter swriter;
        Json::Value sendJson = passParam[moduleParam->sendStr];
        std::string messageStr = swriter.write(sendJson);
        const char* messagebody = messageStr.c_str();
        
        DEBUGLOGGER("**sending response:"+messageStr);
        // printf("***sending response:%s\n", messagebody);
        // printf("**response routing: %.*s\n",
        //      (int)routingkey.len,
        //      (char *)routingkey.bytes);

        amqp_basic_properties_t props;
        props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG | AMQP_BASIC_CORRELATION_ID_FLAG;

        props.content_type = amqp_cstring_bytes("text/plain");
        props.delivery_mode = 2; /* persistent delivery mode */
        props.correlation_id = correlationid;

        if(amqp_basic_publish(connection, channel, amqp_cstring_bytes(exchange),routingkey, 0, 0, &props, amqp_cstring_bytes(messagebody))<0){
            return ERR_THINK_RABBITMQ_SENDMESSAGE_BLOCK;
        }

    } catch (std::exception& e){
        std::cout<<e.what()<<std::endl;
        throw;
        return ERR_THINK_RABBITMQ_SENDMESSAGE_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_SENDMESSAGE_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::SendHeartBeat(think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam, ERROR_CODE ret){
    //check is error happened at last STEP
    if(CErrorCode::isErrorHappened(ret)){
        return ret;
    }

    try{
        amqp_channel_t channel = (amqp_channel_t)moduleParam->channel; 
        amqp_connection_state_t connection = moduleParam->connection;

        amqp_frame_t heartbeat;
        heartbeat.channel = 0;
        heartbeat.frame_type = AMQP_FRAME_HEARTBEAT;

        int res = amqp_send_frame(connection, &heartbeat);
        if (AMQP_STATUS_OK != res) {
            return ERR_THINK_RABBITMQ_HEARTBEAT_BLOCK;
        }
        
    } catch (std::exception& e){
        throw;
        return ERR_THINK_RABBITMQ_HEARTBEAT_EXCEPTION;
    } catch (...){
        ERRORLOGGER("unknown exception, maybe Segmentation fault (core dumped)");
        throw;
        return ERR_THINK_RABBITMQ_HEARTBEAT_EXCEPTION;
    }

    return ERR_OK;
}

ERROR_CODE think::Crabbitmq::DoStart(think::Crabbitmq::ACTION action, think::Crabbitmq::ModuleParam_t* moduleParam, Json::Value& passParam, Json::Value& returnParam){
    ERROR_CODE ret = ERR_OK;
    switch(action){
        case CONNECT:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            break;
        case DISCONNECT:
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        
        case DECLAREEXCHANGE:
            ret = this->DeclareExchange(moduleParam, passParam, returnParam, ret);
            break;
        case DECLAREQUEUE:
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            break;
        case ACKMESSAGE:
            ret = this->MessageACK(moduleParam, passParam, returnParam, ret);
            break;
        case QOSPREFETCH:
            ret = this->QosPrefetch(moduleParam, passParam, returnParam, ret);
            break;

        case SENDMESSAGE:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->SendMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case GETMESSAGE:
            ret = this->GetMessage(moduleParam, passParam, returnParam, ret);
            break;
        case SENDRESPONSEMESSAGE:
            ret = this->SendResponseMessage(moduleParam, passParam, returnParam, ret);
            break;
        
        case SENDMESSAGEONCE:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->SendMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case GETMESSAGEONCE:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareExchange(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            ret = this->QosPrefetch(moduleParam, passParam, returnParam, ret);
            ret = this->GetMessage(moduleParam, passParam, returnParam, ret);
            ret = this->UnbindQueue(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;

        case SENDHEARTBEAT:
            ret = this->SendHeartBeat(moduleParam, passParam, returnParam, ret);
            break;
        
        //for test
        case SENDMESSAGEFORQUEUE:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            ret = this->SendMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case GETMESSAGEFORQUEUE:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            ret = this->GetMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case SENDMESSAGEDISTRIBUTE:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            ret = this->SendMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case GETMESSAGEDISTRIBUTE:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            ret = this->GetMessage(moduleParam, passParam, returnParam, ret);
            ret = this->MessageACK(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case SENDMESSAGESUBSCRIBE:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->SendMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case GETMESSAGESUBSCRIBE:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareExchange(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            ret = this->GetMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case SENDMESSAGEROUTING:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->SendMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case GETMESSAGEROUTING:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareExchange(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            ret = this->GetMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case SENDMESSAGEROUTINGRPC:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareRPCQueue(moduleParam, passParam, returnParam, ret);
            ret = this->SendMessage(moduleParam, passParam, returnParam, ret);
            //ret = this->DeclareRPCQueue(moduleParam, passParam, returnParam, ret);
            ret = this->GetResponseMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case SENDMESSAGETOPIC:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->SendMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case GETMESSAGETOPIC:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareExchange(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            ret = this->GetMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case SENDMESSAGERPC:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareRPCQueue(moduleParam, passParam, returnParam, ret);
            ret = this->SendMessage(moduleParam, passParam, returnParam, ret);
            //ret = this->DeclareRPCQueue(moduleParam, passParam, returnParam, ret);
            ret = this->GetResponseMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case GETMESSAGERPC:
            ret = this->Connection(moduleParam, passParam, returnParam, ret);
            ret = this->DeclareQueue(moduleParam, passParam, returnParam, ret);
            ret = this->GetMessage(moduleParam, passParam, returnParam, ret);
            ret = this->SendResponseMessage(moduleParam, passParam, returnParam, ret);
            ret = this->DisConnection(moduleParam, passParam, returnParam, ret);
            break;
        case NONE:
            break;
    }
    return ret;
}

ERROR_CODE think::Crabbitmq::Start(int action, void* param, Json::Value& passParam, Json::Value& returnParam){
    think::Crabbitmq::ModuleParam_t* moduleParam = (think::Crabbitmq::ModuleParam_t*)param;
    think::Crabbitmq::ACTION actions = (think::Crabbitmq::ACTION)action;

    return this->DoStart(actions, moduleParam, passParam, returnParam);
}

