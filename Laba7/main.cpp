#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>
using namespace std; using namespace chrono;
// Stable Quick Sort: groups preserve the original order of equal values.
void stableQuickSort(vector<int>& a) {
 if (a.size() <= 1) return;
 int pivot = a[a.size()/2];
 vector<int> l, e, g;
 l.reserve(a.size()); e.reserve(a.size()); g.reserve(a.size());
 for(int x:a){if(x<pivot)l.push_back(x);else if(x>pivot)g.push_back(x);else e.push_back(x);} stableQuickSort(l);stableQuickSort(g);a.clear();a.reserve(l.size()+e.size()+g.size());a.insert(a.end(),l.begin(),l.end());a.insert(a.end(),e.begin(),e.end());a.insert(a.end(),g.begin(),g.end());
}
void merge(vector<int>&a,int left,int mid,int right){vector<int>t;t.reserve(right-left+1);int i=left,j=mid+1;while(i<=mid&&j<=right){if(a[i]<=a[j])t.push_back(a[i++]);else t.push_back(a[j++]);}while(i<=mid)t.push_back(a[i++]);while(j<=right)t.push_back(a[j++]);for(int k=0;k<(int)t.size();k++)a[left+k]=t[k];}
void mergeSort(vector<int>&a,int l,int r){if(l>=r)return;int m=l+(r-l)/2;mergeSort(a,l,m);mergeSort(a,m+1,r);merge(a,l,m,r);}
void heapify(vector<int>&a,int n,int root){int largest=root,left=2*root+1,right=2*root+2;if(left<n&&a[left]>a[largest])largest=left;if(right<n&&a[right]>a[largest])largest=right;if(largest!=root){swap(a[root],a[largest]);heapify(a,n,largest);}}
void heapSort(vector<int>&a){int n=(int)a.size();for(int i=n/2-1;i>=0;i--)heapify(a,n,i);for(int i=n-1;i>0;i--){swap(a[0],a[i]);heapify(a,i,0);}}
void printArray(const vector<int>&a){for(int x:a)cout<<x<<' ';cout<<'\n';}
template<class F> long long measure(const vector<int>&src,F sortFunc){vector<int>c=src;auto s=high_resolution_clock::now();sortFunc(c);auto e=high_resolution_clock::now();return duration_cast<microseconds>(e-s).count();}
int main(){cout<<"Лабораторная работа №7. Сравнение сортировок\nВариант 5, продвинутый уровень\n\n";vector<int>sample={64,34,25,12,22,11,90,5,34,12};cout<<"Исходный массив: ";printArray(sample);auto q=sample;stableQuickSort(q);cout<<"Стабильная быстрая сортировка: ";printArray(q);auto m=sample;mergeSort(m,0,(int)m.size()-1);cout<<"Сортировка слиянием:           ";printArray(m);auto h=sample;heapSort(h);cout<<"Пирамидальная сортировка:       ";printArray(h);cout<<"\nПроверка корректности: "<<(q==m&&m==h&&is_sorted(q.begin(),q.end())?"успешно":"ошибка")<<'\n';cout<<"\nСравнение времени на случайных массивах:\n"<<left<<setw(12)<<"Размер"<<setw(22)<<"Quick Sort, мкс"<<setw(22)<<"Merge Sort, мкс"<<"Heap Sort, мкс\n";mt19937 rng(42);for(int n:{1000,5000,10000,20000}){vector<int>d(n);uniform_int_distribution<int>dist(0,n*10);for(int&x:d)x=dist(rng);cout<<left<<setw(12)<<n<<setw(22)<<measure(d,[](auto&v){stableQuickSort(v);})<<setw(22)<<measure(d,[](auto&v){mergeSort(v,0,(int)v.size()-1);})<<measure(d,[](auto&v){heapSort(v);})<<'\n';}cout<<"\nПримечание: время зависит от компьютера и нагрузки системы.\n";}
