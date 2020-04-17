#include <cqcppsdk/cqcppsdk.hpp>

#include "cqfun.hpp"
#include "myJson.hpp"
#include "gui.hpp"



MyJson conf;
std::string appDir;

using namespace cq;



void init() try{
    conf.file2json();
    conf.all2read();
} catch (exception &e) {
    logging::error("init", e.what());
}



CQ_INIT 
    {
        on_enable([] 
            {
                appDir = get_app_directory();
                init();
            });


        on_group_message([] ( const GroupMessageEvent &e )
            {   
                if( msfun::is_adminer(e.user_id) )
                if( msfun::is_switch(e) ) { return; }
                if( !msfun::is_enablegrp(e.group_id) ) { return; }
                if( !msfun::is_command(e) ) { return; }
            });

        on_private_message([] ( const PrivateMessageEvent &e )
            {   
                if (e.sub_type == PrivateMessageEvent::SubType::GROUP) { return; }
                if (msfun::is_register(e) ) { return; }
                if ( msfun::is_check(e) ) { return; }
            });




        on_disable([] {
            conf.all2write();
            conf.json2file();
        });
    }




CQ_MENU(menu_demo_1)
    {
        MainWin win;
        win.openWin();
    }


    
