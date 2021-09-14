all: HTTPServer 

HTTPServer: HTTPServer.cpp
	g++ -g -lpthread -o httpd HTTPServer.cpp _public.cpp

clean:
	rm httpd 
