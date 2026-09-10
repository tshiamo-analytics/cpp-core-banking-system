# 🏦 Core Banking System Prototype

## 🔍 Overview
A robust, console-based core banking engine developed in modern C++17. The system models enterprise banking operations using object-oriented principles, featuring dual-role authentication (Bank Teller and Customer), file-based persistence via binary serialization, transaction tracking, and batch interest calculations.

## 🛠️ Tools Used
* C++17
* GCC / MinGW (`g++`)
* VS Code & PowerShell
* Mermaid.js (UML Design)

## 📌 Key System Features
* **Role-Based Portals:** Independent authentication environments for Branch Tellers and Customers.
* **Polymorphic Account Framework:** Abstract account architecture driving specialized accounts (Savings, Cheque, and Fixed Deposit) with distinct minimum balances and interest tiers.
* **Transactional Persistence:** Direct binary serialization into `.dat` files ensuring data integrity across application sessions.
* **Security & Credential Hashing:** Custom hashing routines preventing plaintext credential storage for teller access and customer PINs.
* **Batch Operations & Reporting:** Automated monthly interest accrual, cross-branch comparison metrics, and structured CSV data exports.

## 💡 C++ Concepts Applied
* **Object-Oriented Programming:** Inheritance, polymorphism, virtual functions, and abstract classes.
* **File Stream Serialization:** Binary read/write modes (`std::ios::binary`) using `ifstream` and `ofstream`.
* **Standard Template Library (STL):** Dynamic memory management with `std::vector` and runtime pointer arrays.
* **Input Validation & Defensive Coding:** Input stream error management via `cin.fail()`, buffer flushing, and account lockout protection.

## 📁 Files Included
* `main.cpp` - Complete banking application source code
* `system_config.txt` - System and branch configuration parameters
* `branches.dat` - Serialized branch metadata
* `tellers.dat` - Serialized teller credentials and security hashes
* `customers.dat` - Serialized account records and active ledger balances
* `transactions.dat` - Comprehensive transaction ledger for statement audits
