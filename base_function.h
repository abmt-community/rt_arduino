/*
 * base_function.h
 *
 *  Created on: 16.05.2015
 *      Author: hva
 */

#ifndef BASE_FUNCTION_H_
#define BASE_FUNCTION_H_

template <typename T>
class function;

template <typename Result, typename... Args>
class function<Result(Args...)> {
private:
	// Dieses Interface wird von function<> benutzt
	// und schreibt vor wie sich ein Funktionsobjekt zu verhalten hat
    struct interface {
        virtual Result operator()(Args...) = 0;
        virtual ~interface(){};
        virtual interface* get_copy() = 0;
    };

    // Implementierung der Schnittstelle (interface):
    // Kopiert(move) das übergebene Funktionsobjekt.
    template <typename T>
    struct wrapper : interface {
    	T f_obj;
    	template <typename U>
        wrapper(U&& u) : f_obj(std::forward<U>(u)) {}

        Result operator()(Args... a) {
        	return f_obj(std::forward<Args>(a)...);
        }
        interface* get_copy(){
        	//cast to const to force copy_constructor
        	return new wrapper<T>((const wrapper<T>&) *this);
        }
    };

    // Das Funktionsobjekt:
    // Durch das Interface ist es Typ-Unabhängig
    interface* fn;

public:
    // Im Konstruktor findet die Reduktion des Funktions-Objekts statt.
    // Das Funktionsobjekt wird vom Wrapper gehalten und auf den Typ Interface reduziert.
    // Die Reduktion auf interface get T verloren. function<> kennt T nicht bzw interface ist unhabhänig von T.
    template <typename T>
    function(T&& obj):fn(new wrapper<typename std::decay<T>::type>(std::forward<T>(obj)) ){}

    function(){
    	fn = 0;
    }

    //Zuweisungsoperator (wie Kostruktor)
    template <typename T>
    function<Result(Args...)>& operator=(T obj){
    	delete fn;
    	fn = new wrapper<typename std::decay<T>::type>(std::forward<T>(obj));
    	return *this;
    }

    Result operator()(Args... args) const {
    	if (fn != 0){
    		return (*fn)(std::forward<Args>(args)...);
    	}else{
    		//std::cout << "uninitialisiert" << std::endl;
    	}
    }

    //rule of three
    //todo: move-semantik
    function(function<Result(Args...)>& f){
		fn = f.fn->get_copy();
		//std::cout << "function: copy construct" << std::endl;
	}
    function<Result(Args...)>& operator=(function<Result(Args...)>& f  ){
		fn = f.fn->get_copy();
		//std::cout << "function: copy_assignment" << std::endl;
		return *this;
	}
    ~function(){
		delete fn;
	}
};

#endif /* BASE_FUNCTION_H_ */
