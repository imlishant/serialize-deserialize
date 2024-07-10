#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

class Object {
public:
    virtual std::vector<char> serialize() const = 0;
    virtual void deserialize(const std::vector<char>& data, size_t& index) = 0;
};

class Employee : public Object {
public:
    std::string name;
    float salary;

    std::vector<char> serialize() const override {
        std::vector<char> data;
        size_t size = name.size();
        data.insert(data.end(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(size));
        data.insert(data.end(), name.begin(), name.end());
        data.insert(data.end(), reinterpret_cast<const char*>(&salary), reinterpret_cast<const char*>(&salary) + sizeof(salary));
        return data;
    }

    void deserialize(const std::vector<char>& data, size_t& index) override {
        size_t size = *reinterpret_cast<const size_t*>(&data[index]);
        index += sizeof(size);
        name.assign(data.begin() + index, data.begin() + index + size);
        index += size;
        salary = *reinterpret_cast<const float*>(&data[index]);
        index += sizeof(salary);
    }
};

class Customer : public Object {
public:
    std::string name;
    std::string address;

    std::vector<char> serialize() const override {
        std::vector<char> data;
        size_t size = name.size();
        data.insert(data.end(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(size));
        data.insert(data.end(), name.begin(), name.end());
        size = address.size();
        data.insert(data.end(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(size));
        data.insert(data.end(), address.begin(), address.end());
        return data;
    }

    void deserialize(const std::vector<char>& data, size_t& index) override {
        size_t size = *reinterpret_cast<const size_t*>(&data[index]);
        index += sizeof(size);
        name.assign(data.begin() + index, data.begin() + index + size);
        index += size;
        size = *reinterpret_cast<const size_t*>(&data[index]);
        index += sizeof(size);
        address.assign(data.begin() + index, data.begin() + index + size);
        index += size;
    }
};

class Item : public Object {
public:
    std::string name;
    float amount;

    std::vector<char> serialize() const override {
        std::vector<char> data;
        size_t size = name.size();
        data.insert(data.end(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(size));
        data.insert(data.end(), name.begin(), name.end());
        data.insert(data.end(), reinterpret_cast<const char*>(&amount), reinterpret_cast<const char*>(&amount) + sizeof(amount));
        return data;
    }

    void deserialize(const std::vector<char>& data, size_t& index) override {
        size_t size = *reinterpret_cast<const size_t*>(&data[index]);
        index += sizeof(size);
        name.assign(data.begin() + index, data.begin() + index + size);
        index += size;
        amount = *reinterpret_cast<const float*>(&data[index]);
        index += sizeof(amount);
    }
};

class FoodItem : public Item {
public:
    float foodCouponDiscount;

    std::vector<char> serialize() const override {
        std::vector<char> data = Item::serialize();
        data.insert(data.end(), reinterpret_cast<const char*>(&foodCouponDiscount), reinterpret_cast<const char*>(&foodCouponDiscount) + sizeof(foodCouponDiscount));
        return data;
    }

    void deserialize(const std::vector<char>& data, size_t& index) override {
        Item::deserialize(data, index);
        foodCouponDiscount = *reinterpret_cast<const float*>(&data[index]);
        index += sizeof(foodCouponDiscount);
    }
};

class Sale : public Object {
public:
    Customer* customer;
    Employee* saleRep;
    std::vector<Item*> items;
    float totalAmount;
    float totalFoodCouponDiscount;
    std::time_t date;

    std::vector<char> serialize() const override {
        std::vector<char> data;
        size_t size = sizeof(date);
        data.insert(data.end(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(size));
        data.insert(data.end(), reinterpret_cast<const char*>(&date), reinterpret_cast<const char*>(&date) + sizeof(date));
        size = items.size();
        data.insert(data.end(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(size));
        for (const auto& item : items) {
            std::vector<char> itemData = item->serialize();
            size = itemData.size();
            data.insert(data.end(), reinterpret_cast<const char*>(&size), reinterpret_cast<const char*>(&size) + sizeof(size));
            data.insert(data.end(), itemData.begin(), itemData.end());
        }
        data.insert(data.end(), reinterpret_cast<const char*>(&totalAmount), reinterpret_cast<const char*>(&totalAmount) + sizeof(totalAmount));
        data.insert(data.end(), reinterpret_cast<const char*>(&totalFoodCouponDiscount), reinterpret_cast<const char*>(&totalFoodCouponDiscount) + sizeof(totalFoodCouponDiscount));
        return data;
    }

    void deserialize(const std::vector<char>& data, size_t& index) override {
        size_t size = *reinterpret_cast<const size_t*>(&data[index]);
        index += sizeof(size);
        date = *reinterpret_cast<const std::time_t*>(&data[index]);
        index += sizeof(date);
        size = *reinterpret_cast<const size_t*>(&data[index]);
        index += sizeof(size);
        items.clear();
        for (size_t i = 0; i < size; ++i) {
            size_t itemSize = *reinterpret_cast<const size_t*>(&data[index]);
            index += sizeof(itemSize);
            std::vector<char> itemData(data.begin() + index, data.begin() + index + itemSize);
            index += itemSize;
            Item* item = nullptr;
            if (itemData[0] == 'F') {
                item = new FoodItem();
            } else {
                item = new Item();
            }
            size_t itemIndex = 0;
            item->deserialize(itemData, itemIndex);
            items.push_back(item);
        }
        totalAmount = *reinterpret_cast<const float*>(&data[index]);
        index += sizeof(totalAmount);
        totalFoodCouponDiscount = *reinterpret_cast<const float*>(&data[index]);
        index += sizeof(totalFoodCouponDiscount);
    }
};

class Store {
// private:

public:

    std::vector<Employee> employees;
    std::vector<Customer> customers;
    std::vector<Sale> sales;
    std::vector<Item*> items;

// public:


    void addEmployee() {
        Employee employee;
        std::cout << "Enter employee name: ";
        std::cin >> employee.name;
        std::cout << "Enter employee salary: ";
        std::cin >> employee.salary;
        employees.push_back(employee);
    }

    void addCustomer() {
        Customer customer;
        std::cout << "Enter customer name: ";
        std::cin >> customer.name;
        std::cout << "Enter customer address: ";
        std::cin >> customer.address;
        customers.push_back(customer);
    }

    void addItem() {
        Item* item;
        char type;
        std::cout << "Enter item type (I for regular item, F for food item): ";
        std::cin >> type;
        if (type == 'F') {
            item = new FoodItem();
        } else {
            item = new Item();
        }
        std::cout << "Enter item name: ";
        std::cin >> item->name;
        std::cout << "Enter item amount: ";
        std::cin >> item->amount;
        if (type == 'F') {
            FoodItem* foodItem = static_cast<FoodItem*>(item);
            std::cout << "Enter food coupon discount: ";
            std::cin >> foodItem->foodCouponDiscount;
        }
        items.push_back(item);
    }

    void serialize() {
        std::ofstream file("Data.bin", std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for writing." << std::endl;
            return;
        }

        // Serialize employees
        size_t size = employees.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& employee : employees) {
            std::vector<char> data = employee.serialize();
            size = data.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size));
            file.write(data.data(), size);
        }

        // Serialize customers
        size = customers.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& customer : customers) {
            std::vector<char> data = customer.serialize();
            size = data.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size));
            file.write(data.data(), size);
        }

        // Serialize sales
        size = sales.size();
        file.write(reinterpret_cast<const char*>(&size), sizeof(size));
        for (const auto& sale : sales) {
            std::vector<char> data = sale.serialize();
            size = data.size();
            file.write(reinterpret_cast<const char*>(&size), sizeof(size));
            file.write(data.data(), size);
        }

        file.close();
    }

    void deserialize() {
        std::ifstream file("Data.bin", std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open file for reading." << std::endl;
            return;
        }

        // Deserialize employees
        size_t size;
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        employees.clear();
        for (size_t i = 0; i < size; ++i) {
            file.read(reinterpret_cast<char*>(&size), sizeof(size));
            std::vector<char> data(size);
            file.read(data.data(), size);
            Employee employee;
            size_t index = 0;
            employee.deserialize(data, index);
            employees.push_back(employee);
        }

        // Deserialize customers
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        customers.clear();
        for (size_t i = 0; i < size; ++i) {
            file.read(reinterpret_cast<char*>(&size), sizeof(size));
            std::vector<char> data(size);
            file.read(data.data(), size);
            Customer customer;
            size_t index = 0;
            customer.deserialize(data, index);
            customers.push_back(customer);
        }

        // Deserialize sales
        file.read(reinterpret_cast<char*>(&size), sizeof(size));
        sales.clear();
        for (size_t i = 0; i < size; ++i) {
            file.read(reinterpret_cast<char*>(&size), sizeof(size));
            std::vector<char> data(size);
            file.read(data.data(), size);
            Sale sale;
            size_t index = 0;
            sale.deserialize(data, index);
            sales.push_back(sale);
        }

        file.close();
    }

    void loadFile() {
        deserialize();
        if (employees.empty() && customers.empty() && sales.empty()) {
            std::cout << "No data found in the file." << std::endl;
        } else {
            std::cout << "Data loaded successfully." << std::endl;
        }
    }

    void writeFile() {
        serialize();
        std::cout << "Data written to file successfully." << std::endl;
    }

};

int main() {
    Store store;

    // Add employees/customers/items and end the program
    store.addEmployee();
    store.addCustomer();
    store.addItem();


    // Save data to file
    store.writeFile();

    // Clear data from memory
    store.employees.clear();
    store.customers.clear();
    store.items.clear();

    // Load data from file
    store.loadFile();

    // Display loaded data
    std::cout << "Employees:" << std::endl;
    for (const auto& employee : store.employees) {
        std::cout << "Name: " << employee.name << ", Salary: " << employee.salary << std::endl;
    }

    std::cout << "Customers:" << std::endl;
    for (const auto& customer : store.customers) {
        std::cout << "Name: " << customer.name << ", Address: " << customer.address << std::endl;
    }

    std::cout << "Items:" << std::endl;
    for (const auto& item : store.items) {
        std::cout << "Name: " << item->name << ", Amount: " << item->amount;
        if (dynamic_cast<const FoodItem*>(item)) {
            std::cout << ", Food Coupon Discount: " << static_cast<const FoodItem*>(item)->foodCouponDiscount;
        }
        std::cout << std::endl;
    }    



    return 0;

}