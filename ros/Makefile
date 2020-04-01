ALL: pub sub

.PHONY:pub
pub:
	g++ -O3 -g3 -o pub pub.cpp -I/opt/ros/kinetic/include -L/opt/ros/kinetic/lib -lrosconsole -lrostime -lcpp_common -lroscpp_serialization -lroscpp -std=c++11

.PHONY:sub
sub:
	g++ -O3 -g3 -o sub sub.cpp -I/opt/ros/kinetic/include -L/opt/ros/kinetic/lib -lrosconsole -lrostime -lcpp_common -lroscpp_serialization -lroscpp -std=c++11

.PHONY:clean
clean:
	rm -rf pub sub
