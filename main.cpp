#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>  
#include <stdlib.h>  

struct Node {
    int time;
    int x;
    int y;
};

typedef struct Lift {
    int end_x, end_y;
    int travel_time;
    int interval;
    struct Lift* next;
} Lift;

typedef struct {
    int width;
    int height;
    int** grid;
    int** dist;
    Lift*** lifts;
} MapData;

void HeapPush(Node* heap, int& heapSize, int time, int x, int y);
Node MinElement(Node* heap, int& heapSize);
MapData* CreateMapData(int width, int height);
Node* InitHeap(int heapMaxSize, MapData* mapData);
void FreeMap(int** map, int height);
void FreeDist(int** dist, int height);
void FreeLifts(Lift*** lifts, int height, int width);
void Dijkstra(MapData* mapData, Node* heap, int end_x, int end_y, int& heapSize, int heapMaxSize);
void DijkstraLift(MapData* mapData, Node* heap, int& heapSize, int heapMaxSize, Node current, bool** visited);

void HeapPush(Node* heap, int& heapSize, int time, int x, int y) {
    heap[heapSize].time = time;
    heap[heapSize].x = x;
    heap[heapSize].y = y;
    int pos = heapSize++;

    while (pos > 0) {
        int parent = (pos - 1) / 2;
        if (heap[pos].time < heap[parent].time) {
            Node temp = heap[pos];
            heap[pos] = heap[parent];
            heap[parent] = temp;
            pos = parent;
        }
        else break;
    }

}

Node MinElement(Node* heap, int& heapSize) {
    Node min = heap[0];
    heap[0] = heap[--heapSize];

    int pos = 0;
    while (true) {
        int left = 2 * pos + 1;
        int right = 2 * pos + 2;
        int smallest = pos;

        if (left < heapSize && heap[left].time < heap[smallest].time)
            smallest = left;
        if (right < heapSize && heap[right].time < heap[smallest].time)
            smallest = right;

        if (smallest != pos) {
            Node temp = heap[pos];
            heap[pos] = heap[smallest];
            heap[smallest] = temp;

            pos = smallest;
        }
        else break;
    }
    return min;
}

MapData* CreateMapData(int width, int height) {
    MapData* mapData = (MapData*)malloc(sizeof(MapData));
    mapData->width = width;
    mapData->height = height;

    mapData->grid = (int**)malloc(height * sizeof(int*));
    mapData->dist = (int**)malloc(height * sizeof(int*));
    mapData->lifts = (Lift***)malloc(height * sizeof(Lift**));

    for (int y = 0; y < height; y++) {
        mapData->grid[y] = (int*)malloc(width * sizeof(int));
        mapData->dist[y] = (int*)malloc(width * sizeof(int));
        mapData->lifts[y] = (Lift**)calloc(width, sizeof(Lift*));

        for (int x = 0; x < width; x++) {
            mapData->dist[y][x] = INT_MAX;
        }
    }
    return mapData;
}

Node* InitHeap(int heapMaxSize, MapData* mapData) {
    Node* heap = (Node*)malloc(heapMaxSize * sizeof(Node));
    if (heap == NULL) {
        FreeMap(mapData->grid, mapData->height);
        FreeDist(mapData->dist, mapData->height);
        FreeLifts(mapData->lifts, mapData->height, mapData->width);
        free(mapData);
    }
    return heap;
}

void Dijkstra(MapData* mapData, Node* heap, int end_x, int end_y, int& heapSize, int heapMaxSize) {
    const int dx[] = { -1, 1, 0, 0 };
    const int dy[] = { 0, 0, -1, 1 };

    bool** visited = (bool**)malloc(mapData->height * sizeof(bool*));
    for (int y = 0; y < mapData->height; y++) {
        visited[y] = (bool*)calloc(mapData->width, sizeof(bool));
    }

    while (heapSize > 0) {
        Node current = MinElement(heap, heapSize);

        if (visited[current.y][current.x]) continue;
        visited[current.y][current.x] = true;

        if (current.x == end_x && current.y == end_y) {
            printf("%d\n", current.time);
            for (int y = 0; y < mapData->height; y++) free(visited[y]);
            free(visited);
            return;
        }

        for (int i = 0; i < 4; i++) {
            int new_x = current.x + dx[i];
            int new_y = current.y + dy[i];

            if (new_x < 0 || new_x >= mapData->width) continue;
            if (new_y < 0 || new_y >= mapData->height) continue;
            if (visited[new_y][new_x]) continue;

            int current_height = mapData->grid[current.y][current.x];
            int neighbor_height = mapData->grid[new_y][new_x];
            int cost = (neighbor_height > current_height) ?
                (neighbor_height - current_height + 1) : 1;

            int new_time = current.time + cost;

            if (new_time < mapData->dist[new_y][new_x]) {
                mapData->dist[new_y][new_x] = new_time;
                if (heapSize < heapMaxSize) {
                    HeapPush(heap, heapSize, new_time, new_x, new_y);
                }
            }
        }

        DijkstraLift(mapData, heap, heapSize, heapMaxSize, current, visited);
    }

    for (int y = 0; y < mapData->height; y++) free(visited[y]);
    free(visited);
}

void DijkstraLift(MapData* mapData, Node* heap, int& heapSize, int heapMaxSize, Node current, bool** visited) {
    Lift* lift = mapData->lifts[current.y][current.x];
    while (lift != NULL) {
        int departure_time = current.time;

        if (lift->interval > 0) {
            while (departure_time % lift->interval != 0) {
                departure_time++;
            }
        }

        int arrival_time = departure_time + lift->travel_time;

        if (!visited[lift->end_y][lift->end_x]) {
            if (arrival_time < mapData->dist[lift->end_y][lift->end_x]) {
                mapData->dist[lift->end_y][lift->end_x] = arrival_time;
                if (heapSize < heapMaxSize) {
                    HeapPush(heap, heapSize, arrival_time, lift->end_x, lift->end_y);
                }
            }
        }

        lift = lift->next;
    }
}


void FreeMap(int** map, int height) {
    if (map == NULL) return;

    for (int i = 0; i < height; i++) {
        free(map[i]);
    }
    free(map);
}

void FreeDist(int** dist, int height) {
    if (dist == NULL) return;

    for (int i = 0; i < height; i++) {
        free(dist[i]);
    }
    free(dist);
}

void FreeLifts(Lift*** lifts, int height, int width) {
    if (!lifts) return;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Lift* lift = lifts[y][x];
            while (lift != NULL) {
                Lift* temp = lift;
                lift = lift->next;
                free(temp);
            }
        }
        free(lifts[y]);
    }
    free(lifts);
}


int main() {
    int height, width;
    if (scanf("%d %d", &width, &height) != 2) {  
           return 1;  
       }

    MapData* mapData = CreateMapData(width, height);
    if (!mapData) {
        return 1;
    }

    int start_x, start_y, end_x, end_y;
    if (scanf("%d %d %d %d", &start_x, &start_y, &end_x, &end_y) != 4) {
        FreeMap(mapData->grid, height);
        FreeDist(mapData->dist, height);
        FreeLifts(mapData->lifts, height, width);
        free(mapData);
        return 1;
    }


    // Lifts init
    int L;
    if (scanf("%d", &L) != 1) return 1;
    for (int i = 0; i < L; i++) {
        int sx, sy, ex, ey, T, D;
        if (scanf("%d %d %d %d %d %d", &sx, &sy, &ex, &ey, &T, &D) != 6) {
            FreeMap(mapData->grid, mapData->height);
            FreeDist(mapData->dist, mapData->height);
            FreeLifts(mapData->lifts, mapData->height, mapData->width);
            free(mapData);
            exit(1);
        }

        Lift* new_lift = (Lift*)malloc(sizeof(Lift));
        new_lift->end_x = ex;
        new_lift->end_y = ey;
        new_lift->travel_time = T;
        new_lift->interval = D;
        new_lift->next = mapData->lifts[sy][sx];
        mapData->lifts[sy][sx] = new_lift;
    }

    // Map input
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (scanf("%d", &mapData->grid[y][x]) != 1) {
                FreeMap(mapData->grid, mapData->height);
                FreeDist(mapData->dist, mapData->height);
                FreeLifts(mapData->lifts, mapData->height, mapData->width);
                free(mapData);
                exit(1);
            }
        }
    }

    // Heap init
    int heapMaxSize = width * height;
    int heapSize = 0;
    Node* heap = InitHeap(heapMaxSize, mapData);
    if (heap == NULL) {
        return 1;
    }

    mapData->dist[start_y][start_x] = 0;

    HeapPush(heap, heapSize, 0, start_x, start_y);
    Dijkstra(mapData, heap, end_x, end_y, heapSize, heapMaxSize);

    FreeMap(mapData->grid, height);
    FreeDist(mapData->dist, height);
    FreeLifts(mapData->lifts, height, width);
    free(heap);
    free(mapData);
    return 0;
}