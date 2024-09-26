#include <iostream>
#include <string>
#include "../includes/models.hpp"
#include <cctype>
#include <typeinfo>
#include <cxxabi.h>
#include <cstdlib>
#include <vector>
using namespace std;

string to_upper(const string& str){
    for(char& ch : str){
        ch = toupper(ch);
    }
    return str;
}
string demangle(T& obj){
    char* mangled_name = typeid(obj).name();
    int status = 0;

    char* demangled_name = abi::__cxa_demangle(mangled_name, nullptr, nullptr, &status);
    string result;

    if(status == 0){
        result = demangled_name;
    }else{
        result = mangled_name;
        cerr<<"Demangling failed ::"<<result<<endl;
    }
    free(demangled_name);
    return result;
}


Model::Model(){model_count++;}//counts the number of models the user has created...will be useful in the following fxn

Model::create_table(T& obj){
    string model_name = demangle(obj);
    create_call_count++;
    vector<string> primary_key_cols;
    vector<string> sql_strings;
    unordered_map<string, T> fields = obj.fields;
    for(const auto& [col, col_obj] : fields){
        if(col_obj.datatype == "BOOLEAN" || col_obj.datatype == "BYTEA" || col_obj.datatype == "FOREIGN KEY"){
            if(col_obj.datatype == "FOREIGN KEY"){
                string fk_sql_seg = col_obj.sql_generator(col);
                sql_strings.push_back(fk_sql_seg);
            }
            string full_sql_segment = col + col_obj.sql_segment;
            sql_strings.push_back(full_sql_segment);
        }
        if(col_obj.primary_key){
            primary_key_cols.push_back(col);
            string full_sql_segment = col + col_obj.sql_segment;
            sql_strings.push_back(full_sqpl_segment);
        }
        if(col_obj.datatype == "INTEGER" || col_obj.datatype == "SMALLINT" || col_obj.datatype == "BIGINT"){
            if(col_obj.check_condition != "default"){
                string cond = col_obj.check_condition, sql_seg;
                switch (cond){
                    case "gte":
                        sql_seg = "CHECK (" + col + ">=" + to_string(check_constraint) + ")";
                        break;
                    case "lte":
                        sql_seg = "CHECK (" + col + "<=" + to_string(check_constraint) + ")";
                        break;
                    case "gt":
                        sql_seg = "CHECK (" + col + ">" + to_string(check_constraint) + ")";
                        break;
                    case "lt":
                        sql_seg = "CHECK (" + col + "<" + to_string(check_constraint) + ")";
                        break;
                    case "net":
                        sql_seg = "CHECK (" + col + "!=" + to_string(check_constraint) + ")";
                        break;
                    case "et":
                        sql_seg = "CHECK (" + col + "=" + to_string(check_constraint) + ")";
                        break;
                    default:
                        cerr<<"No such check condition "<<cond<<" was found" <<endl;
                }
                sql_strings.push_back(sql_seg);
            }
        }
        string full_sql_segment = col + col_obj.sql_segment;
        sql_strings.push_back(full_sql_segment);
    }
    tables[model_name] = sql_strings;
    if(create_call_count == model_count){
        //orm_fxn(tables, primary_key_cols);
        //this is a fxn that accepts the formatted sql strings and the model name and then inserts into the database
        break;
    }
    //we also need a fxn to check for changes in the models and react accordingly. will be implemented later.
}

IntegerField::IntegerField(string datatype, bool not_null, bool unique, bool pk, int check_constr, string check_cond):
    datatype(datatype), not_null(not_null), unique(unique), primary_key(pk), check_constraint(check_constr), check_condition(check_cond)
 {
    sql_generator();
 }

void IntegerField::sql_generator(){
    datatype = to_upper(datatype);
    if(datatype != "INTEGER" || datatype != "SMALLINT"||datatype != "BIGINT"){
        cerr<< "Datatype" <<datatype<<"is not supporte by postgreSQL. Provide a valid datatype"<<endl;
    }
    sql_segment += datatype;
    if (not_null) sql_segment.append(" " + "NOT NULL");
    if (unique) sql_segment.append(" " + "UNIQUE");
}

DecimalField::DecimalField(string datatype, int max_length, int decimal_places, bool pk):
    datatype(datatype), max_length(max_length), decimal_places(decimal_places), primary_key(pk)
{
    sql_generator();
}

void DecimalField::sql_generator(){
    datatype = to_upper(datatype);
    if(datatype != "DECIMAL" || datatype != "REAL"||datatype != "DOUBLE PRECISION" || datatype != "NUMERIC"){
        cerr<< "Datatype" <<datatype<<"is not supporte by postgreSQL. Provide a valid datatype"<<endl;
    }
    sql_segment = datatype + "(" + to_string(max_size) + "," + to_string(decimal_places) + ")";
}

CharField::CharField(string datatype, bool not_null, bool unique, int length, bool pk):
    datatype(datatype), not_null(not_null), unique(unique), length(length), primary_key(pk)
{
    sql_generator();
}
void CharField::sql_generator(){
    datatype = to_upper(datatype);
    if(datatype != "VARCHAR" || datatype != "CHAR"||datatype != "TEXT"){
        cerr<< "Datatype" <<datatype<<"is not supporte by postgreSQL. Provide a valid datatype"<<endl;
    }
    sql_segment += datatype;
    if (length == 0){
        if(datatype != "TEXT")cerr<< "Length attribute is required for datatype " << datatype << endl;
    }
    sql_segment.append("(" + to_string(length) + ")");
    if (not_null) sql_segment.append(" " + "NOT NULL"); 
    if (unique) sql_segment.append(" " + "UNIQUE");
}

BoolField::BoolField(bool not_null, bool enable_default, bool default_value): 
    not_null(not_null), enable_default(enable_default), default_value(default_value)
{
    sql_generator();
}

void BoolField::sql_generator(){
    sql_segment += datatype;
    if (not_null)sql_segment.append(" " + "NOT NULL");
    if (enable_default)sql_segment.append(" " + "DEFAULT" + (default_value ? "TRUE" : "FALSE"));
}

BinaryField::BinaryField(int size, bool not_null, bool unique, bool pk):size(size), not_null(not_null), unique(unique), primary_key(pk)
{
    sql_generator();
}
void BinaryField::sql_generator(){ 
    if(size == 0)cerr<< "Size cannot be zero for datatype "<<datatype<<endl;
    sql_segment = datatype + "(" + to_string(size) + ")";
    if(not_null) sql_segment.append(" " + "NOT NULL");
    if(unique) sql_segment .append(" " + "UNIQUE");
}

DateTimeField::DateTimeField(string datatype, bool pk, bool enable_default, string default_val):
    datatype(datatype), primary_key(pk), enable_default(enable_default), default_val(default_val)
{
    sql_generator();
}
void DateTimeField::sql_generator(){
    datatype = to_upper(datatype);
    if(datatype != "DATE" || datatype != "TIME" || datatype != "TIMESTAMP_WTZ" || datatype != "TIMESTAMP" || datatype != "TIME_WTZ" || datatype != "INTERVAL") cerr<<"Datatype " <<datatype<< "not supported in postgreSQL. Provide a valid datatype"<<endl;
    
    sql_segment += datatype;
    if(enable_default){
        default_val = to_upper(default_val);
        sql_segment.append(" " + "DEFAULT" + default_val);
    }
}

string ForeignKey::sql_generator(column){
    string sql_segment = datatype + " " + "(" + column + ")" + "REFERENCES" + model_name + "(" + column_name + ")";
    if(on_delete){
        on_delete = to_upper(on_delete);
        sql_segment.append("ON DELETE" + " " + on_delete);
    } 
    if(on_update){
        on_update = to_upper(on_update);
        sql_segment.append("ON UPDATE" + " " + on_update);
    }
    return sql_segment;
}
