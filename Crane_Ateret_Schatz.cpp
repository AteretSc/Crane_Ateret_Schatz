#include <iostream> 
#include <fstream>
#include <string> 
#include <vector>
#include <cmath>
//#include<stdio.h>
using namespace std;


struct state {
   long double R;//R [m]- distance between the crane's trolley to the crane's mast
   long double slew;//slew [deg]- slewing angle of the crane's jib
   long double h;//h [m]- height of the hook from the ground

   /* state operator= (const state& a)
    {
        state res;
        res.R = a.R;
        res.slew = a.slew;
        res.h = a.h;
        return res;
    }*/
};

/*int main()
{
	//pointXYZ point1;
	pcl::PointCloud<pcl::PointXYZ> cloud;
	std::vector<pcl::PointXYZ> data = cloud.points;
}*/


/*
void Slicing_axis(long double** matrix, int size_matrix, int axis) {
    string str = "Dev_DATA/out_one_axis.txt";// csv";
    ofstream of_axis(str.c_str());
    if (axis < 1 or axis>3)
    {
        cout << "error axis";
    }
    for (int i = 0; i < size_matrix; i++)
    {
        of_axis << matrix[i][axis]<<endl;
    }
    of_axis.close();
}*/

long double calculate_distance(long double x_1, long double y_1, long double z_1,
    long double x_2, long double y_2, long double z_2)
{
    return sqrt((x_1 - x_2)*(x_1 - x_2) + (y_1 - y_2)*(y_1 - y_2) + (z_1 - z_2)*(z_1 - z_2));
}

int main()
{
    cout << "I take the next input from file_input file. If you want to change value, this is the time" << endl;
    cout << "\nfirst_line: Slicing - which axi - 1=x, 2=y, 3=z";
    cout << "\nsecont line: Min distance between neighbores ";
    cout << "\nthird line: N - how many frame files to take together";//
    cout << "\nfourth line: The Speed_exceeds"<<endl;
    cout << "DONT FORGET TO CLOSE THE input_file NOW. press 1 and enter to proceed" << endl;
    
    int temp_enter;
    cin >> temp_enter;

    string str_file_input = "Dev_DATA/inputs.txt";
    ifstream file_input(str_file_input.c_str());
    if (!file_input)
    {
        cerr << "input file name is incorrect" << endl;   exit(1);
    }

    getline(file_input, str_file_input);
    int axis = (int)atof(str_file_input.c_str());

    getline(file_input, str_file_input);
    long double neighboring = (long double)atof(str_file_input.c_str());

    getline(file_input, str_file_input);
    int N = (int)atof(str_file_input.c_str());

    getline(file_input, str_file_input);
    long double Speed_exceeds = (long double)atof(str_file_input.c_str());

    //frame
    string str_frame = "Dev_DATA/PC/frame";
    string str_file_frame;
    ifstream file_frame;
    ofstream file_frame_out;

    //state hendler
    string file;
    string file_temp = "Dev_DATA/State/state";
    ifstream state_file;
    state fir_state, sec_state;
    string temp;
    ofstream out_log("Dev_DATA/Log.txt");
    
    long double distance = 0;
    long double speed = 0;
    long double deg=0,deg1=0, deg2 = 0;
    //AC^2 = AB^2 + AC^2 - 2*AB*AC*cos a
    //long double ABAB = 0, ACAC = 0, BCBC = 0;
    long double X1=0, Y1=0, Z1=0;
    long double X2 = 0, Y2 = 0, Z2 = 0;
    for (int i = 1; i <= 757; i++)//757
    {


        file = file_temp + to_string(i) + ".csv";
        state_file.open(file.c_str());
        if (!state_file)
        {
            cerr << "input file name is incorrect " << file << endl;   exit(1);
        }
        getline(state_file, temp, ',');
        fir_state.R = (long double)atof(temp.c_str());
        getline(state_file, temp, ',');
        fir_state.slew = (long double)atof(temp.c_str());//slew_handle((long double)atof(temp.c_str()));
        getline(state_file, temp, ',');
        fir_state.h = (long double)atof(temp.c_str());

        //cout << "R: " << fir_state.R << " slew: " << fir_state.slew << " h: " << fir_state.h << endl;

        distance = 0;
        speed = 0;
     //   deg = 0;
    //    ABAB = ACAC = BCBC = 0;
        //AC^2 = AB^2 + AC^2 - 2*AB*AC*cos a

        if (i > 1)
        {
            Y1 = fir_state.h;
            Y2 = sec_state.h;

            deg1 = fir_state.slew * 3.141592 / 180;//to radians
            deg2 = sec_state.slew * 3.141592 / 180;//to radians
            
            X1 = fir_state.R * cos(deg1);
            X2 = sec_state.R * cos(deg2);

            Z1 = fir_state.R * sin(deg1);
            Z2 = sec_state.R * sin(deg2);

            distance = calculate_distance(X1, Y1, Z1, X2, Y2, Z2);

            if (distance < 0.0000001) {
                distance = 0;
            }
            else {
                //distance = sqrt(BCBC);
                out_log << "change of state. new state: R: " << sec_state.R << " slew: " << sec_state.slew << " h: " << sec_state.h << "\n";
            }
            
            speed = distance / 0.2;
            deg = deg1 - deg2;
            if (deg < 0) {
                deg = deg * -1;
            }
            while (deg > 360)//עם עשה סיבוב אחד או יותר
            {
                speed = speed + 2 * 3.141592 * sec_state.R;//maybe fir_state.R - sec_state.R??
                deg = deg - 360;
            }
            if (speed > Speed_exceeds) {
                out_log << "Speed_exceeds! The speed change is: " << speed << "\n";
                
                str_file_frame = str_frame + to_string(i) + ".csv";
                file_frame.open(str_file_frame.c_str(), std::ofstream::out | std::ofstream::trunc);//DELETE all the PC in the frame that the speed exceeds;
                if (!file_frame)
                {
                    cerr << "input file name is incorrect: " << str_file_frame << endl;   exit(1);
                }
                file_frame.close();
            }
            else if (speed != 0) {
                out_log << "The speed change is: " << speed << "\n";
            }

           

        }
        sec_state.R = fir_state.R;
        sec_state.slew = fir_state.slew;
        sec_state.h = fir_state.h;

        state_file.close();
       

    }
    out_log.close();




    
    int lines = 0;
    string line;
    for (int j = 1; j <= 757; j=j+N)
    {
        lines = 0;
        for (int i = 1; i <= N; i++)
        {
            str_file_frame = str_frame + to_string(j) + ".csv";
            ifstream file_frame(str_file_frame.c_str());
            if (!file_frame)
            {
                cerr << "input file name is incorrect: " << str_file_frame << endl;   exit(1);
            }
            //count number of line for frame1
            for (; getline(file_frame, line);)
            {
                if (line != "")
                    lines++;
            }
            //cout << lines << endl;
            file_frame.close();
        }
       // cout << lines << endl;

        long double** matrix_frame1 = new long double* [lines];
        long double distimation_neighbore = 0;
        int num_of_lines = lines;
        //Data batch – ability to perform slicing / clustering / inspection on a batch of N point clouds
        for (int i = 1; i <= N; i++)
        {
            str_file_frame = str_frame + to_string(j) + ".csv";
            ifstream file_frame(str_file_frame.c_str());
            if (!file_frame)
            {
                cerr << "input file name is incorrect: " << str_file_frame << endl;   exit(1);
            }

            for (lines = 0; lines < num_of_lines; lines++)
            {
                matrix_frame1[lines] = new long double[3];
                getline(file_frame, line, ','); 
                
                if (line == "")//if (file_frame.eof())//אם הקובץ הסתיים
                {
                    i = i + 1;
                    break;// breack from this for;
                }

                
                matrix_frame1[lines][1] = (long double)atof(line.c_str());//x
                getline(file_frame, line, ',');
                matrix_frame1[lines][2] = (long double)atof(line.c_str());//y
                getline(file_frame, line, ',');
                matrix_frame1[lines][3] = (long double)atof(line.c_str());//zs


                for (int k = 1; k < lines; k++)
                {
                    distimation_neighbore = calculate_distance(matrix_frame1[lines][1],
                        matrix_frame1[lines][2], matrix_frame1[lines][3],
                        matrix_frame1[k][1], matrix_frame1[k][2], matrix_frame1[k][3]);
                    if (distimation_neighbore < neighboring)// clustering – the ability to cluster neighboring points 
                    {
                        lines = lines - 1;
                        num_of_lines = num_of_lines - 1;
                    }
                }
            }
            file_frame.close();
        }


       // Slicing_axis(matrix_frame1, lines, axis);//x=1, y=2, z=3;
        string str = "Dev_DATA/out_one_axis_N";// csv";
        string str1 = str + to_string(j) + ".txt";
        ofstream of_axis(str1.c_str());
        if (axis < 1 or axis>3)
        {
            cout << "error axis";
        }
        for (int i = 0; i < lines; i++)
        {
            of_axis << matrix_frame1[i][axis] << endl;
        }
        of_axis.close();

        //for (int i = 0; i < lines; ++i) {
         //   delete[] matrix_frame1[i];
        //}
        //delete[]matrix_frame1;

    }
    
    return 0;
}

