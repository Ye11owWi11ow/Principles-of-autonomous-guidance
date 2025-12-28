#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

struct ValidationRule {
    string field;
    double min, max;
};

class DataValidator {
private:
    vector<ValidationRule> rules;
    vector<string> errors;
    int totalChecks = 0;
    int passedChecks = 0;

public:
    void addValidationRule(const string& field, double min, double max) {
        rules.push_back({field, min, max});
    }

    bool validateCoordinates(double x, double y, double z) {
        totalChecks++;
        bool valid = true;

        for (const auto& rule : rules) {
            if (rule.field == "x" && (x < rule.min)) {
                errors.push_back("Coordinates: ERROR - X coordinate " + to_string(x) +
                                 " exceeds minimum -10000.0");
                valid = false;
            }
            if (rule.field == "x" && (x > rule.max)) {
                errors.push_back("Coordinates: ERROR - X coordinate " + to_string(x) +
                                 " exceeds maximum 10000.0");
                valid = false;
            }
            if (rule.field == "y" && (y < rule.min)) {
                errors.push_back("Coordinates: ERROR - Y coordinate " + to_string(y) +
                                 " exceeds minimum -10000.0");
                valid = false;
            }
            if (rule.field == "y" && (y > rule.max)) {
                errors.push_back("Coordinates: ERROR - Y coordinate " + to_string(y) +
                                 " exceeds maximum 10000.0");
                valid = false;
            }
            if (rule.field == "z" && (z < rule.min)) {
                errors.push_back("Coordinates: ERROR - height " + to_string(z) +
                                 " exceeds minimum 0");
                valid = false;
            }
            if (rule.field == "z" && (z > rule.max)) {
                errors.push_back("Coordinates: ERROR - height " + to_string(z) +
                                 " exceeds maximum 5000.0");
                valid = false;
            }
        }
        if (valid) passedChecks++;
        return valid;
    }

    bool validateSpeed(double speed) {
        totalChecks++;
        bool valid = true;

        for (const auto& rule : rules) {
            if (rule.field == "speed" && (speed < rule.min)) {
                errors.push_back("Speed: ERROR - speed " + to_string(speed) +
                                 " exceeds minimum 0");
                valid = false;
            }
        }

        for (const auto& rule : rules) {
            if (rule.field == "speed" && (speed > rule.max)) {
                errors.push_back("Speed: ERROR - speed " + to_string(speed) +
                                 " exceeds maximum 300.0");
                valid = false;
            }
        }

        if (valid) passedChecks++;
        return valid;
    }

    bool validateAcceleration(double acceleration) {
        totalChecks++;
        bool valid = true;

        for (const auto& rule : rules) {
            if (rule.field == "acceleration" && (acceleration < rule.min)) {
                errors.push_back("Acceleration: ERROR - acceleration " + to_string(acceleration) +
                                 " exceeds minimum -50.0");
                valid = false;
            }
        }

        for (const auto& rule : rules) {
            if (rule.field == "acceleration" && (acceleration > rule.max)) {
                errors.push_back("Acceleration: ERROR - acceleration " + to_string(acceleration) +
                                 " exceeds maximum 20.0");
                valid = false;
            }
        }

        if (valid) passedChecks++;
        return valid;
    }

    void generateValidationReport(const string& filename) {
        ofstream fout(filename);
        fout << "Validation Report:\n";
        fout << "==================\n\n";

        if (errors.empty()) {
            fout << "All checks passed successfully!\n";
        } else {
            fout << "Validation errors found:\n";
            for (const auto& err : errors) {
                fout << "  • " << err << "\n";
            }
        }

        fout << "\nStatistics:\n";
        fout << "Total checks performed: " << totalChecks << "\n";
        fout << "Passed checks: " << passedChecks << "\n";
        fout << "Failed checks: " << (totalChecks - passedChecks) << "\n";
        fout << "Validation score: " << fixed << setprecision(1) << getValidationScore() << "%\n";

        if (getValidationScore() == 0) {
            fout << "\nOverall result: 0% of data is valid\n";
        }

        fout.close();
        cout << "Report saved to " << filename << endl;
    }

    double getValidationScore() {
        if (totalChecks == 0) return 100.0;
        return (passedChecks * 100.0) / totalChecks;
    }

    void printErrors() {
        if (errors.empty()) {
            cout << "All checks passed!\n";
        } else {
            cout << "Validation errors:\n";
            for (const auto& err : errors) {
                cout << "  - " << err << "\n";
            }
        }
        cout << "Validation score: " << fixed << setprecision(1) << getValidationScore() << "%\n";
    }
};

void createSampleData() {
    // Data from condition
    cout << "\nTest data from condition:\n";
    cout << "Coordinates: x=5000.5, y=3000.2, z=10000.0 (invalid: z > 5000)\n";
    cout << "Speed: 350.0 (invalid: > 300)\n";
    cout << "Acceleration: 25.0 (invalid: > 20)\n";
}

int main() {
    DataValidator validator;

    // Add validation rules
    validator.addValidationRule("x", -10000, 10000);
    validator.addValidationRule("y", -10000, 10000);
    validator.addValidationRule("z", 0, 5000);      // Max 5000 from condition
    validator.addValidationRule("speed", 0, 300);   // Max 300 from condition
    validator.addValidationRule("acceleration", -50, 20); // Max 20 from condition

    // Test with data from condition
    createSampleData();

    // Validate coordinates (invalid: z > 5000)
    cout << "\nValidating coordinates...\n";
    validator.validateCoordinates(5000.5, 3000.2, 10000.0);

    // Validate speed (invalid: > 300)
    cout << "Validating speed...\n";
    validator.validateSpeed(350.0);

    // Validate acceleration (invalid: > 20)
    cout << "Validating acceleration...\n";
    validator.validateAcceleration(25.0);

    // Also test some valid data
    cout << "\nTesting valid data...\n";
    validator.validateCoordinates(1000.0, 2000.0, 3000.0);
    validator.validateSpeed(250.0);
    validator.validateAcceleration(15.0);

    // Show results
    validator.printErrors();

    // Generate report
    validator.generateValidationReport("validation_report.txt");

    return 0;
}
