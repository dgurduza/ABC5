#include <iostream>
#include <random>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ConcurrentQueue.h"

ConcurrentQueue patients_queue;
ConcurrentQueue dentist_patients;
ConcurrentQueue therapist_patients;
ConcurrentQueue surgeon_patients;
std::mutex mutex_for_surgeon_thread;
std::mutex mutex_for_therapist_thread;
std::mutex mutex_for_dentist_thread;
std::mutex mutex;
std::condition_variable surgeon_object;
std::condition_variable therapist_object;
std::condition_variable dentist_object;

void reception(int number_of_doctor) {
    for (;patients_queue.size() != 0;) {
        std::unique_lock<std::mutex> main_thread_locker(mutex);
        std::string name = patients_queue.front();
        patients_queue.pop();
        std::cout << name << " came to the registry to see the doctor ¹" << std::to_string(number_of_doctor) << "\n";
        int index_of_doctor = rand() % 3 + 1;
        main_thread_locker.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        if (index_of_doctor == 1) {
            main_thread_locker.lock();
            std::cout << "The doctor ¹" << std::to_string(number_of_doctor) << " sent " << name << "to therapist" << "\n";
            main_thread_locker.unlock();
            therapist_patients.push(name);
            therapist_object.notify_all();
        }
        else if (index_of_doctor == 2) {
            main_thread_locker.lock();
            std::cout << "The doctor ¹" << std::to_string(number_of_doctor) << " sent " << name << "to dentist" << "\n";
            main_thread_locker.unlock();
            dentist_patients.push(name);
            dentist_object.notify_all();
        }
        else if (index_of_doctor == 3) {
            main_thread_locker.lock();
            std::cout << "The doctor ¹" << std::to_string(number_of_doctor) << " sent " << name << "to surgeon" << "\n";
            main_thread_locker.unlock();
            surgeon_patients.push(name);
            surgeon_object.notify_all();
        }
    }
}

void runTherapistWork() {
    for (;;) {
        if (therapist_patients.size() == 0) {
            std::unique_lock<std::mutex> therapist_thread_locker(mutex_for_therapist_thread);
            therapist_object.wait_for(therapist_thread_locker, std::chrono::seconds(5));
            if (patients_queue.size() == 0 && therapist_patients.size() == 0) {
                return;
            }
        }
        if (therapist_patients.size() != 0) {
            std::unique_lock<std::mutex> main_thread_locker(mutex);
            std::string name_of_therapist_patient = therapist_patients.front();
            therapist_patients.pop();
            std::cout << name_of_therapist_patient << " patient at a therapist's appointment\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
            std::cout << name_of_therapist_patient << " appointment with the therapist is over\n";
            main_thread_locker.unlock();
        }
    }
}

void runDentistWork() {
    for (;;) {
        if (dentist_patients.size() == 0) {
            std::unique_lock<std::mutex> dentist_thread_locker(mutex_for_dentist_thread);
            dentist_object.wait_for(dentist_thread_locker, std::chrono::seconds(5));
            if (patients_queue.size() == 0 && dentist_patients.size() == 0) {
                return;
            }
        }
        if (dentist_patients.size() != 0) {
            std::unique_lock<std::mutex> main_thread_locker(mutex);
            std::string name_of_dentist_patient = dentist_patients.front();
            dentist_patients.pop();
            std::cout << name_of_dentist_patient << " patient at a dentist's appointment\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
            std::cout << name_of_dentist_patient << " appointment with the dentist is over\n";
            main_thread_locker.unlock();
        }
    }
}

void runSurgeonWork() {
    for (;;) {
        if (surgeon_patients.size() == 0) {
            std::unique_lock<std::mutex> surgeon_thread_locker(mutex_for_surgeon_thread);
            surgeon_object.wait_for(surgeon_thread_locker, std::chrono::seconds(5));
            if (patients_queue.size() == 0 && surgeon_patients.size() == 0) {
                return;
            }
        }
        if (surgeon_patients.size() != 0) {
            std::unique_lock<std::mutex> main_thread_locker(mutex);
            std::string name_of_surgeon_patient = surgeon_patients.front();
            surgeon_patients.pop();
            std::cout << name_of_surgeon_patient << " patient at a surgeon's appointment\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(750));
            std::cout << name_of_surgeon_patient << " appointment with the surgeon is over\n";
            main_thread_locker.unlock();
        }
    }
}

int main() {
    std::string custom;
    do {
        std::cout << "Enter \"rand\" for random padding of visitors or \"cons\" for console padding \n";
        std::cin >> custom;
    } while (custom != "rand" && custom != "cons");

    if (custom == "cons") {
        std::string value;
        for (;;) {
            std::cout << "Enter the names of the visitors. To finish typing, write \"next\" \n";
            std::cin >> value;
            if (value != "next") {
                patients_queue.push(value);
            }
            else {
                break;
            }
        }
    }
    else {
        int number;
        std::cout << "Enter the number of visitors (up to 10,000) \n";
        std::cin >> number;
        if (number > 0 && number < 10001) {
            for (int i = 1;i <= number;++i) {
                patients_queue.push("Patient ¹ " + std::to_string(i));
            }
        }
        else {
            std::cout << "Invalid input\n";
            return 1;
        }
    }

    std::thread first_doctor_on_reception(reception, 1);
    std::thread second_doctor_on_reception(reception, 2);
    std::thread therapist(runTherapistWork);
    std::thread dentist(runDentistWork);
    std::thread surgeon(runSurgeonWork);

    first_doctor_on_reception.join();
    second_doctor_on_reception.join();
    therapist.join();
    dentist.join();
    surgeon.join();

    std::cout << "The clinic is closed!\n";

    return 0;
}