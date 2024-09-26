#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <unordered_map>
//#include "./queryclass.hpp"

using namespace std;

template <typename T>
class Model{
    static int model_count = 0;
    int create_call_count = 0; 
public:
    unordered_map<string, vector<string>>tables; //under review, bcoz a string is better for memory than a map.
    //QueryClass records;

    Model();//used for counting the number of models created
            //this will be used by the create_table fxn to count the number of times its been called
            //and when it matched the number of models, we pass the map to an orm fxn.

    void create_table(T& obj);// demangler and mangler, takes the model map, concats the key(col_name) with obj.sql_segment
                              // increments the create_call_count variable 
                              //check for primary keys and if present, creates, otherwise uses a default 'id' pk
                              //checks for foreign keys and creates, checks for constraints eq, 'check' and also inserts those into the
                              //vector. after all these, it inserts the demangled model name as key and vector as value. 
                              //checks if we have matched model_count, if we have, we pass the map to an orm fxn, else we cotinue.
};


class IntegerField{
    string datatype, sql_segment, check_condition;
    bool not_null, unique, primary_key;
    int check_constraint;

public: 
    IntegerField(string datatype = "INT", bool not_null = false, bool unique = false, bool pk = false, int check_constr = 0,
                 string check_cond = "default"); 
    void sql_generator();
};

class DecimalField{
    string datatype, sql_segment;
    int max_length, decimal_places;
    bool primary_key;
    
public:
    DecimalField(string datatype = "DECIMAL", int max_length = 10, int decimal_places = 0, bool pk = false);
    void sql_generator();
};

class CharField{
    string datatype, sql_segment;
    int length;
    bool not_null, unique, primary_key;

public:
    CharField(string datatype = "VARCHAR", bool not_null = false, bool unique = false, int length = 0, bool pk = false);
    void sql_generator();
};

class BoolField{
    string datatype = "BOOLEAN";
    string sql_segment;
    bool enable_default,default_value, not_null, unique;

public:
    BoolField(bool not_null = false, bool enable_default = false, bool default_value = false);
    void sql_generator();
};

class BinaryField{
    string datatype = "BYTEA";
    string sql_segment; 
    int size;
    bool not_null, unique, primary_key;

public:
    BinaryField(int size = 1, bool not_null = false, bool unique = true, bool pk = false);
    void sql_generator();
};

class DateTimeField{
    string datatype, sql_segment, default_val, on_update;
    bool enable_default, primary_key;

public:
    DateTimeField(string datatype = "TIMESTAMP", bool pk = false, bool enable_default = false, string default_val = "CURRENT_TIMESTAMP");
    void sql_generator();
};

class ForeignKey{
    string datatype = "FOREIGN KEY", model_name, column_name, on_update, on_delete;

public:
    ForeignKey(string model_name, string column_name, string on_delete, string on_update):
        model_name(model_name), column_name(column_name), on_update(on_update), on_delete(on_delete)
        {}
    string sql_generator(string column);
};

#endif 
