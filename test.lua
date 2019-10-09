
sd=require("sigstackdump")


sd.setsignal('SIGUSR2')
sd.setdump(print)


n=20


function f()
    g()
end

function g()
    h()
end


function h()
    while n> 0 do
        os.execute("sleep 1")
        n=n-1
    end
end



f()
