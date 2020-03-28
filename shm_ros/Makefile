ALL: pub sub

.PHONY:pub
pub:
	g++ -o pub pub.cpp -I/opt/plusai/drive/include/common -I/opt/ros/kinetic/include -std=c++14 -ldrive_common -L/opt/plusai/lib -lglog -lroscpp -L/opt/ros/kinetic/lib -lrosconsole -lroscpp_serialization -lcpp_common -lrostime -Wl,-rpath=/opt/plusai/external/opt/lib -Wl,-rpath=/opt/plusai/lib

.PHONY:sub
sub:
	g++ -o sub sub.cpp -I/opt/plusai/drive/include/common -I/opt/ros/kinetic/include -std=c++14 -ldrive_common -L/opt/plusai/lib -lglog -lroscpp -L/opt/ros/kinetic/lib -lrosconsole -lroscpp_serialization -lcpp_common -lrostime -Wl,-rpath=/opt/plusai/external/opt/lib -lboost_system -Wl,-rpath=/opt/plusai/lib

.PHONY:clean
clean:
	rm -rf pub sub
