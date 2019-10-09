


sigstackdump.so: sigstackdump.c
	gcc -o $@ -fPIC -shared $^ -Ilua-5.1.5/include -Llua-5.1.5/lib/ -Bstatic -llua 

clean:
	rm *.so
