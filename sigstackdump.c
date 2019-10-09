#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

static lua_State * s_lua_state = NULL;
static int s_last_signal = 0;
extern const char *const sys_siglist[];
static char s_bt_msg[1024*4];

const char * sig_name_list [] = { "-",
"HUP","INT","QUIT","ILL","TRAP","ABRT","BUS","FPE", "KILL", "USR1",
"SEGV", "USR2" ,"PIPE", "ALRM", "TERM", "STKFLT", "CHLD", "CONT", 
"STOP", "TSTP", "TTIN", "TTOU", "URG", "XCPU", "XFSZ", "VTALRM", 
"PROF" ,"WINCH", "POLL", "PWR", "SYS", NULL};


static void signal_dump(int sig){
    if(s_lua_state){
        int btl = snprintf(s_bt_msg, sizeof(s_bt_msg)-1, "traceback stack:\n");
        lua_Debug ldb;
        int i;
        const char * name, * filename;
        int line;
        for(i=1;lua_getstack(s_lua_state, i, &ldb) == 1; i++){
            lua_getinfo(s_lua_state, "Slnu", &ldb);
            name = ldb.name;
            if(!name){
                name = "--";
            }
            line = 0;
            if(ldb.currentline > 0){
                line = ldb.currentline;
            }
            filename = ldb.short_src;
            btl += snprintf(s_bt_msg+btl, sizeof(s_bt_msg)-btl-1, "[%d][%s]'%s':%d @%s\n", i+1-1, ldb.what, filename, line, name);
        }

        lua_getglobal(s_lua_state,"_signal_stack_dump_print_func");
        lua_pushstring(s_lua_state, s_bt_msg);
        lua_pcall(s_lua_state, 1, 0, 0);
    }
}

typedef void (*sighandler_t)(int);
static void set_signal(isig){
    if(s_last_signal > 0 ){
        signal(s_last_signal, SIG_DFL);
        s_last_signal = 0;
    }
    if(isig <= 0){
        return ;
    }
    sighandler_t si = signal(isig, signal_dump); 
    if (si == SIG_ERR){
        fprintf(stderr,"signal set errno=%d strerror:%s\n", errno, strerror(errno));
    }
    else {
        s_last_signal = isig;
    }
}


static int l_signal(lua_State * lua){
    const char * sig = luaL_checkstring(lua, 1);
    if (sig == NULL ){
        luaL_error(lua, "error signal param");
        return 0;
    }

    if(strcmp(sig, "0") == 0){
        set_signal(0);
        lua_pushboolean(lua,1);
        return 1;
    }

    int isig = SIGUSR1;
    int i=-1;
    for(i = 1;i < 32 && sig_name_list[i]; ++i){
        if (strcasecmp(sig_name_list[i], sig+3) == 0){
            isig = i;
            break;
        }
    }
    if(i == -1){
        lua_pushboolean(lua,0);
        return 1;
    }
    set_signal(isig);
    lua_pushboolean(lua,1);
    return 1;
}


static const struct luaL_Reg _regs[] = {
    {"signal", l_signal},
    {NULL, NULL},
};

int luaopen_sigstackdump_c(lua_State* lua){
    s_lua_state = lua;
    luaL_register(lua, "sigstackdump.c", _regs);
    return 1;

}
