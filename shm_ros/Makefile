ALL: pub sub

.PHONY:pub
pub:
	g++ -O3 -g3 -o pub pub.cpp -I/opt/plusai/drive/include -I/opt/plusai/drive/include/common -I/opt/ros/kinetic/include -std=c++14 -ldrive_common -ldrive_proto -L/opt/plusai/lib -lboost_system -lgflags -lglog -lprotobuf -lroscpp -L/opt/ros/kinetic/lib -lrosconsole -lroscpp_serialization -lcpp_common -lrostime -Wl,-rpath=/opt/plusai/external/opt/lib -Wl,-rpath=/opt/plusai/lib

.PHONY:sub
sub:
	g++ -O3 -g3 -o sub sub.cpp -I/opt/plusai/drive/include -I/opt/plusai/drive/include/common -I/opt/ros/kinetic/include -std=c++14 -ldrive_common -ldrive_proto -L/opt/plusai/lib -lboost_system -lgflags -lglog -lprotobuf -lroscpp -L/opt/ros/kinetic/lib -lrosconsole -lroscpp_serialization -lcpp_common -lrostime -Wl,-rpath=/opt/plusai/external/opt/lib -Wl,-rpath=/opt/plusai/lib

.PHONY:clean
clean:
	rm -rf pub sub
