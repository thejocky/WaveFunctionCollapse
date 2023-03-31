


template <typename T>
struct Link {
    Link* next;
    Link* last;
    T var;
};

template <typename T>
struct ForwardLink {
    ForwardLink* next;
    T var;
};

template <typename T>
class LinkedList {
    typedef typename Link<T> Link;
    Link first_;
    Link last_;

    Link *first() {return first_.next;}
    Link *last() {return last_.last;}

    static Link *pop(Link *link) {
        link->last.next = link->next;
        link->next.last = link->last;
        return link;
    }

    static void appendFollowing(Link *linkInList, Link *link) {
        link->next = linkInList
    }


    void append(T) {}
};
