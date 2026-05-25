#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ITEMS 50
#define MAX_ORDERS 100
#define MAX_STRING 50

// ---------------- STRUCTURES ----------------

typedef struct {
    int item_id;
    char name[MAX_STRING];
    double price;
    char cuisine[MAX_STRING];
    int prep_time;
} MenuItem;

typedef struct {
    int item_id;
    int quantity;
} OrderItem;

typedef struct {
    int order_id;
    int room_number;
    OrderItem items[MAX_ITEMS];
    int item_count;
    char status[MAX_STRING];
    double total_bill;
} Order;

// ---------------- GLOBAL VARIABLES ----------------

MenuItem menu[MAX_ITEMS];
Order orders[MAX_ORDERS];

int menu_count = 0;
int order_count = 0;

// ---------------- FUNCTION PROTOTYPES ----------------

void load_menu();
void load_orders();
void save_orders();

void display_menu();
void place_order();
void add_to_order();
void cancel_from_order();
void generate_bill();
void kitchen_summary();
void update_status();

int generate_unique_id();
int find_menu_item(int id);
int find_order(int id);

// ---------------- MAIN FUNCTION ----------------

int main() {
    int choice;

    srand(time(NULL));

    load_menu();
    load_orders();

    while (1) {

        printf("\n=============================================\n");
        printf(" HOTEL ROOM SERVICE FOOD ORDERING SYSTEM\n");
        printf("=============================================\n");

        printf("1. Display Menu\n");
        printf("2. Place New Order\n");
        printf("3. Add Items to Existing Order\n");
        printf("4. Cancel Items from Order\n");
        printf("5. Generate Bill\n");
        printf("6. Kitchen Summary\n");
        printf("7. Update Order Status\n");
        printf("8. Exit\n");

        printf("---------------------------------------------\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);

        switch (choice) {

            case 1:
                display_menu();
                break;

            case 2:
                place_order();
                break;

            case 3:
                add_to_order();
                break;

            case 4:
                cancel_from_order();
                break;

            case 5:
                generate_bill();
                break;

            case 6:
                kitchen_summary();
                break;

            case 7:
                update_status();
                break;

            case 8:
                save_orders();
                printf("\nData Saved Successfully.\n");
                exit(0);

            default:
                printf("Invalid Choice!\n");
        }
    }

    return 0;
}

// ---------------- FILE FUNCTIONS ----------------

void load_menu() {

    FILE *fp = fopen("menu.txt", "r");

    if (!fp) {

        fp = fopen("menu.txt", "w");

        fprintf(fp, "101,Spring_Rolls,12.50,Starter,10\n");
        fprintf(fp, "102,Paneer_Tikka,15.00,Indian,15\n");
        fprintf(fp, "103,Club_Sandwich,10.00,Continental,8\n");
        fprintf(fp, "104,Hakka_Noodles,14.00,Chinese,12\n");
        fprintf(fp, "105,Molten_Lava_Cake,8.50,Dessert,7\n");

        fclose(fp);

        fp = fopen("menu.txt", "r");
    }

    while (fscanf(fp, "%d,%[^,],%lf,%[^,],%d",
                  &menu[menu_count].item_id,
                  menu[menu_count].name,
                  &menu[menu_count].price,
                  menu[menu_count].cuisine,
                  &menu[menu_count].prep_time) == 5) {

        menu_count++;

        if (menu_count >= MAX_ITEMS)
            break;
    }

    fclose(fp);
}

void load_orders() {

    FILE *fp = fopen("orders.txt", "r");

    if (!fp)
        return;

    while (fscanf(fp, "%d %d %s %lf %d",
                  &orders[order_count].order_id,
                  &orders[order_count].room_number,
                  orders[order_count].status,
                  &orders[order_count].total_bill,
                  &orders[order_count].item_count) == 5) {

        for (int i = 0; i < orders[order_count].item_count; i++) {

            fscanf(fp, "%d %d",
                   &orders[order_count].items[i].item_id,
                   &orders[order_count].items[i].quantity);
        }

        order_count++;

        if (order_count >= MAX_ORDERS)
            break;
    }

    fclose(fp);
}

void save_orders() {

    FILE *fp = fopen("orders.txt", "w");

    if (!fp) {
        printf("Error Saving Orders!\n");
        return;
    }

    for (int i = 0; i < order_count; i++) {

        fprintf(fp, "%d %d %s %.2lf %d",
                orders[i].order_id,
                orders[i].room_number,
                orders[i].status,
                orders[i].total_bill,
                orders[i].item_count);

        for (int j = 0; j < orders[i].item_count; j++) {

            fprintf(fp, " %d %d",
                    orders[i].items[j].item_id,
                    orders[i].items[j].quantity);
        }

        fprintf(fp, "\n");
    }

    fclose(fp);
}

// ---------------- DISPLAY MENU ----------------

void display_menu() {

    printf("\n------------------------------------------------------------\n");
    printf("%-8s %-20s %-10s %-15s %-10s\n",
           "ID", "Name", "Price", "Cuisine", "Prep");

    printf("------------------------------------------------------------\n");

    for (int i = 0; i < menu_count; i++) {

        printf("%-8d %-20s %-10.2lf %-15s %-10d\n",
               menu[i].item_id,
               menu[i].name,
               menu[i].price,
               menu[i].cuisine,
               menu[i].prep_time);
    }

    printf("------------------------------------------------------------\n");
}

// ---------------- PLACE ORDER ----------------

void place_order() {

    Order new_order;

    printf("\nEnter Room Number: ");
    scanf("%d", &new_order.room_number);

    new_order.order_id = generate_unique_id();
    new_order.item_count = 0;
    new_order.total_bill = 0;

    strcpy(new_order.status, "Received");

    char choice = 'y';

    while (choice == 'y' || choice == 'Y') {

        int id, qty;

        display_menu();

        printf("Enter Item ID: ");
        scanf("%d", &id);

        int index = find_menu_item(id);

        if (index == -1) {
            printf("Item Not Found!\n");
            continue;
        }

        printf("Enter Quantity: ");
        scanf("%d", &qty);

        new_order.items[new_order.item_count].item_id = id;
        new_order.items[new_order.item_count].quantity = qty;

        new_order.item_count++;

        new_order.total_bill += menu[index].price * qty;

        printf("Add More Items? (y/n): ");
        scanf(" %c", &choice);
    }

    orders[order_count++] = new_order;

    save_orders();

    printf("\nOrder Placed Successfully!\n");
    printf("Order ID: %d\n", new_order.order_id);
}

// ---------------- ADD TO ORDER ----------------

void add_to_order() {

    int order_id;

    printf("\nEnter Order ID: ");
    scanf("%d", &order_id);

    int ord_index = find_order(order_id);

    if (ord_index == -1) {
        printf("Order Not Found!\n");
        return;
    }

    int item_id, qty;

    display_menu();

    printf("Enter Item ID to Add: ");
    scanf("%d", &item_id);

    int menu_index = find_menu_item(item_id);

    if (menu_index == -1) {
        printf("Item Not Found!\n");
        return;
    }

    printf("Enter Quantity: ");
    scanf("%d", &qty);

    orders[ord_index].items[orders[ord_index].item_count].item_id = item_id;
    orders[ord_index].items[orders[ord_index].item_count].quantity = qty;

    orders[ord_index].item_count++;

    orders[ord_index].total_bill += menu[menu_index].price * qty;

    save_orders();

    printf("Item Added Successfully!\n");
}

// ---------------- CANCEL ITEM ----------------

void cancel_from_order() {

    int order_id;

    printf("\nEnter Order ID: ");
    scanf("%d", &order_id);

    int ord_index = find_order(order_id);

    if (ord_index == -1) {
        printf("Order Not Found!\n");
        return;
    }

    int cancel_id;

    printf("Enter Item ID to Cancel: ");
    scanf("%d", &cancel_id);

    int item_index = -1;

    for (int i = 0; i < orders[ord_index].item_count; i++) {

        if (orders[ord_index].items[i].item_id == cancel_id) {
            item_index = i;
            break;
        }
    }

    if (item_index == -1) {
        printf("Item Not Found in Order!\n");
        return;
    }

    int menu_index = find_menu_item(cancel_id);

    orders[ord_index].total_bill -=
        menu[menu_index].price *
        orders[ord_index].items[item_index].quantity;

    for (int i = item_index;
         i < orders[ord_index].item_count - 1;
         i++) {

        orders[ord_index].items[i] =
            orders[ord_index].items[i + 1];
    }

    orders[ord_index].item_count--;

    save_orders();

    printf("Item Cancelled Successfully!\n");
}

// ---------------- BILL ----------------

void generate_bill() {

    int room;

    printf("\nEnter Room Number: ");
    scanf("%d", &room);

    double total = 0;

    printf("\n=========== BILL ===========\n");

    for (int i = 0; i < order_count; i++) {

        if (orders[i].room_number == room) {

            printf("\nOrder ID: %d\n", orders[i].order_id);

            for (int j = 0; j < orders[i].item_count; j++) {

                int m = find_menu_item(
                    orders[i].items[j].item_id);

                double cost =
                    menu[m].price *
                    orders[i].items[j].quantity;

                printf("%s x%d = %.2lf\n",
                       menu[m].name,
                       orders[i].items[j].quantity,
                       cost);
            }

            printf("Order Total = %.2lf\n",
                   orders[i].total_bill);

            total += orders[i].total_bill;
        }
    }

    printf("\nGrand Total = %.2lf\n", total);
}

// ---------------- KITCHEN SUMMARY ----------------

void kitchen_summary() {

    printf("\n=========== KITCHEN SUMMARY ===========\n");

    for (int i = 0; i < order_count; i++) {

        if (strcmp(orders[i].status, "Received") == 0 ||
            strcmp(orders[i].status, "Preparing") == 0) {

            printf("Order ID: %d | Room: %d | Status: %s\n",
                   orders[i].order_id,
                   orders[i].room_number,
                   orders[i].status);
        }
    }
}

// ---------------- UPDATE STATUS ----------------

void update_status() {

    int order_id;

    printf("\nEnter Order ID: ");
    scanf("%d", &order_id);

    int ord_index = find_order(order_id);

    if (ord_index == -1) {
        printf("Order Not Found!\n");
        return;
    }

    int choice;

    printf("1. Preparing\n");
    printf("2. Served\n");
    printf("3. Cancelled\n");

    printf("Choose Status: ");
    scanf("%d", &choice);

    if (choice == 1)
        strcpy(orders[ord_index].status, "Preparing");

    else if (choice == 2)
        strcpy(orders[ord_index].status, "Served");

    else if (choice == 3)
        strcpy(orders[ord_index].status, "Cancelled");

    else {
        printf("Invalid Choice!\n");
        return;
    }

    save_orders();

    printf("Status Updated Successfully!\n");
}

// ---------------- HELPER FUNCTIONS ----------------

int generate_unique_id() {

    int id;
    int unique;

    do {

        unique = 1;

        id = rand() % 9000 + 1000;

        for (int i = 0; i < order_count; i++) {

            if (orders[i].order_id == id) {
                unique = 0;
                break;
            }
        }

    } while (!unique);

    return id;
}

int find_menu_item(int id) {

    for (int i = 0; i < menu_count; i++) {

        if (menu[i].item_id == id)
            return i;
    }

    return -1;
}

int find_order(int id) {

    for (int i = 0; i < order_count; i++) {

        if (orders[i].order_id == id)
            return i;
    }

    return -1;
}