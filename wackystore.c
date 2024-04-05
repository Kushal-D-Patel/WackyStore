/**
 * Assignment 1 - The Wacky Store
 *
 * Please read the comments below carefully, they describe your task in detail.
 *
 * There are also additional notes and clarifications on Quercus.
 *
 * Any clarifications and corrections will be posted to Piazza so please keep an
 * eye on the forum!
 *
 * Unauthorized distribution or posting is strictly prohibited. You must seek
 * approval from course staff before uploading and sharing with others.
 */

// No additional imports are allowed. You can make helper functions if you wish.
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 1024
#define INT_MAX ((int)((unsigned int)(-1) >> 1))

typedef struct ItemNode ItemNode;
struct ItemNode {
    char name[MAX_NAME_LENGTH];
    int count;
    ItemNode* next;
};

typedef struct Customer Customer;
struct Customer {
    char name[MAX_NAME_LENGTH];
    ItemNode* cart;
};

typedef struct CheckoutLaneNode CheckoutLaneNode;
struct CheckoutLaneNode {
    Customer* customer;

    CheckoutLaneNode* front;
    CheckoutLaneNode* back;
};

typedef struct CheckoutLane CheckoutLane;
struct CheckoutLane {
    CheckoutLaneNode* first;
    CheckoutLaneNode* last;
};

/**
 * Function: new_item_node
 * -----------------------
 * Allocate a new ItemNode. Allocation must be done manually (with malloc or
 * calloc). Initialize all variables using the arguments provided. Assume that
 * count will always be greater than 0.
 */
ItemNode* new_item_node(char* name, int count) {
    ItemNode* newNode = (ItemNode*)malloc(sizeof(ItemNode));
    if (newNode == NULL) {
        exit(EXIT_FAILURE);
    }
    strncpy(newNode->name, name, MAX_NAME_LENGTH - 1);
    newNode->name[MAX_NAME_LENGTH - 1] = '\0';
    newNode->count = count;
    newNode->next = NULL;
    return newNode;
}

/**
 * Function: new_customer
 * ----------------------
 * Allocate a new Customer. Allocation must be done manually (with malloc or
 * calloc). Initialize all variables using the arguments provided.
 */
Customer* new_customer(char* name) {
    Customer* newCustomer = (Customer*)malloc(sizeof(Customer));
    if (newCustomer == NULL) {
        exit(EXIT_FAILURE);
    }
    strncpy(newCustomer->name, name, MAX_NAME_LENGTH - 1);
    newCustomer->name[MAX_NAME_LENGTH - 1] = '\0';
    newCustomer->cart = NULL;
    return newCustomer;
}

/**
 * Function: free_customer
 * -----------------------
 * Release all memory associated with a Customer back to the system. This
 * includes any items they may have had in their cart.
 */
void free_customer(Customer* customer) {
    if (customer == NULL) {
        return;
    }
    ItemNode* current = customer->cart;
    while (current != NULL) {
        ItemNode* next = current->next;
        free(current);
        current = next;
    }
    free(customer);
}

/**
 * Function: open_new_checkout_line
 * --------------------------------
 * Allocate a new empty checkout lane. Allocation must be done manually (with
 * malloc or calloc).
 */
CheckoutLane* open_new_checkout_line() {
    CheckoutLane* newCheckoutLane = (CheckoutLane*)malloc(sizeof(CheckoutLane));
    if (newCheckoutLane == NULL) {
        exit(EXIT_FAILURE);
    }
    newCheckoutLane->first = NULL;
    newCheckoutLane->last = NULL;
    return newCheckoutLane;
}

/**
 * Function: new_checkout_node
 * ---------------------------
 * Allocate a new CheckoutLaneNode. Allocation must be done manually (with
 * malloc or calloc). Initialize all variables using the arguments provided. Do
 * not allocate a new customer; instead copy the existing reference over.
 */
CheckoutLaneNode* new_checkout_node(Customer* customer) {
    CheckoutLaneNode* newNode = (CheckoutLaneNode*)malloc(sizeof(CheckoutLaneNode));
    if (newNode == NULL) {
        exit(EXIT_FAILURE);
    }
    newNode->customer = customer;
    newNode->front = NULL;
    newNode->back = NULL;
    return newNode;
}

/**
 * Function: add_item_to_cart
 * --------------------------
 * Add an item to a customer's cart, given its name and amount.
 *
 * If the given amount is 0 or less, do nothing.
 *
 * IMPORTANT: The items in a customer's cart should always be arranged in
 * lexicographically smallest order based on the item names. Consider the use of
 * the ASCII strcmp() function from <string.h>.
 *
 * No two ItemNodes in a customer's cart can have the same name.
 * If the customer already has an ItemNode with the same item name in their
 * cart, increase the node's count by the given amount instead.
 */
void add_item_to_cart(Customer* customer, char* item_name, int amount) {
    if (amount <= 0) {
        return;
    }

    if (customer == NULL || item_name == NULL) {
        return;
    }

    ItemNode* newItem = new_item_node(item_name, amount);

    if (customer->cart == NULL) {
        customer->cart = newItem;
        return;
    }

    ItemNode* current = customer->cart;
    ItemNode* previous = NULL;
    while (current != NULL && strcmp(item_name, current->name) > 0) {
        previous = current;
        current = current->next;
    }

    if (current != NULL && strcmp(item_name, current->name) == 0) {
        current->count += amount;
        free(newItem);
    } else {
        if (previous == NULL) {
            newItem->next = customer->cart;
            customer->cart = newItem;
        } else {
            previous->next = newItem;
            newItem->next = current;
        }
    }
}

/**
 * Function: remove_item_from_cart
 * -------------------------------
 * Attempt to reduce the quantity of an item in a customer's cart, given its
 * name and amount.
 *
 * If no ItemNode in the customer's cart match the given item name, or the
 * amount given is <= 0, do nothing.
 *
 * If the quantity is reduced to a value less than or equal to 0, remove the
 * ItemNode from the customer's cart. This means you will need to do memory
 * cleanup as well.
 */
void remove_item_from_cart(Customer* customer, char* item_name, int amount) {
     if (amount <= 0 || customer == NULL || item_name == NULL) {
        return;
    }

    if (customer->cart == NULL) {
        return;
    }

    ItemNode* current = customer->cart;
    ItemNode* previous = NULL;
    while (current != NULL && strcmp(item_name, current->name) != 0) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return;
    }

    if (amount >= current->count) {
        if (previous == NULL) {
            customer->cart = current->next;
        } else {
            previous->next = current->next;
        }
        free(current);
    } else {
        current->count -= amount;
    }
}

/**
 * Function: total_number_of_items
 * -------------------------------
 * Count the total number of items in a customer's cart by summing all ItemNodes
 * and their associated quantities.
 */
int total_number_of_items(Customer* customer) {
    int total = 0;

    ItemNode* current = customer->cart;
    while (current != NULL) {
        total += current->count;
        current = current->next;
    }

    return total;
}

/**
 * Function: queue
 * ---------------
 * A customer has finished shopping and wishes to checkout. Add the given
 * customer to the end of the given checkout lane.
 */
void queue(Customer* customer, CheckoutLane* lane) {
    if (customer == NULL || lane == NULL) {
        return;
    }
    
    CheckoutLaneNode* newNode = new_checkout_node(customer);

    if (lane->first == NULL) {
        lane->first = newNode;
        lane->last = newNode;
    } else {
        lane->last->back = newNode;
        newNode->front = lane->last;
        lane->last = newNode;
    }
}

/**
 * Function: process
 * -----------------
 * Serve the customer at the head of the checkout lane and return the
 * total_number_of_items() the customer had in their cart.
 *
 * The customer leaves the store after being processed. Therefore, you must also
 * free any memory associated with them.
 *
 * If this function is called on an empty lane, return 0.
 */
int process(CheckoutLane* lane) {
    if (lane->first == NULL) {
        return 0;
    }

    Customer* customer = lane->first->customer;

    int totalItems = total_number_of_items(customer);

    free_customer(customer);

    CheckoutLaneNode* temp = lane->first;
    lane->first = lane->first->back;
    if (lane->first != NULL) {
        lane->first->front = NULL;
    } else {
        lane->last = NULL;
    }
    free(temp);

    return totalItems;
}

/**
 * Function: balance_lanes
 * -----------------------
 * Move a single customer from the end of the most busy checkout lane to the end
 * of the least busy checkout lane.
 *
 * Busyness is defined as the total number of customers in a checkout lane.
 *
 * If multiple lanes have the same busyness, select the lane that comes first in
 * the CheckoutLane* array.
 *
 * If the difference between the MAX and MIN checkout lanes is <= 1, do nothing.
 *
 * If there are less than 2 lanes, do nothing.
 *
 * Return true if and only if a customer was moved; otherwise false.
 */
bool balance_lanes(CheckoutLane* lanes[], int number_of_lanes) {
    if (number_of_lanes < 2) {
        return false;
    }

    int maxCustomers = 0;
    int minCustomers = INT_MAX;
    int maxIndex = -1;
    int minIndex = -1;

    for (int i = 0; i < number_of_lanes; i++) {
        int customers = 0;
        CheckoutLaneNode* current = lanes[i]->first;
        while (current != NULL) {
            customers++;
            current = current->back;
        }
        if (customers > maxCustomers) {
            maxCustomers = customers;
            maxIndex = i;
        }
        if (customers < minCustomers) {
            minCustomers = customers;
            minIndex = i;
        }
    }

    if (maxCustomers - minCustomers <= 1) {
        return false;
    }

    Customer* movedCustomer = NULL;
    if (lanes[maxIndex]->first != NULL) {
        movedCustomer = lanes[maxIndex]->last->customer;

        CheckoutLaneNode* temp = lanes[maxIndex]->last;
        lanes[maxIndex]->last = lanes[maxIndex]->last->front;
        if (lanes[maxIndex]->last != NULL) {
            lanes[maxIndex]->last->back = NULL;
        } else {
            lanes[maxIndex]->first = NULL;
        }
        free(temp);

        queue(movedCustomer, lanes[minIndex]);
    }

    return (movedCustomer != NULL);
}

/**
 * Function: process_all_lanes
 * ---------------------------
 * Given an array of CheckoutLane*, process() each CheckoutLane a single time
 * and return the the sum of the result.
 */
int process_all_lanes(CheckoutLane* lanes[], int number_of_lanes) {
    int totalItemsProcessed = 0;

    for (int i = 0; i < number_of_lanes; i++) {
        totalItemsProcessed += process(lanes[i]);
    }

    return totalItemsProcessed;
}

/**
 * Function: close_store
 * ---------------------
 * It's closing time. Given an array of CheckoutLane*, free all memory
 * associated with them. Any customers still left in the queue is kicked out and
 * also freed from memory.
 */
void close_store(CheckoutLane* lanes[], int number_of_lanes) {
    for (int i = 0; i < number_of_lanes; i++) {
        CheckoutLane* lane = lanes[i];
        CheckoutLaneNode* current = lane->first;
        while (current != NULL) {
            CheckoutLaneNode* temp = current;
            current = current->back;
            free_customer(temp->customer);
            free(temp);
        }
        free(lane);
    }
}
