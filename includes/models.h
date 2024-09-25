#ifndef models_h
#define models_h

#include <string>
#include <vector>
#include <memory>



class Model {
public:
    Model() = default;
    virtual ~Model() = default;

    
    virtual std::string get_table_name() const = 0;
};

class User : public Model {
public:
    User(int userid, std::string username, std::string email, std::string password_hash)
        : userid(userid), username(std::move(username)), email(std::move(email)), password_hash(std::move(password_hash)) {}

    
    std::string get_table_name() const override {
        return "users";
    }

    
    int get_userid() const { return userid; }
    const std::string& get_username() const { return username; }
    const std::string& get_email() const { return email; }
    const std::string& get_password_hash() const { return password_hash; }

private:
    int userid;
    std::string username;
    std::string email;
    std::string password_hash;
};

class Field {
public:
    virtual ~Field() = default;
    virtual std::string get_sql_type() const = 0;
};

class IntField : public Field {
public:
    IntField(int val = 0) : value(val) {}
    std::string get_sql_type() const override { return "INTEGER"; }

    
    int get_value() const { return value; }
    void set_value(int val) { value = val; }

private:
    int value;
};

class TextField : public Field {
   
public:
    TextField(std::string val = "") : value(std::move(val)) {}
    std::string get_sql_type() const override { return "TEXT"; }

    const std::string& get_value() const { return value; }
    void set_value(std::string val) { value = std::move(val); }
 private:
    std::string value;

};

class BoolField : public Field {
public:
    BoolField(bool val = false) : value(val) {}
    std::string get_sql_type() const override { return "BOOLEAN"; }

   
    bool get_value() const { return value; }
    void set_value(bool val) { value = val; }

private:
    bool value;
};


#endif 