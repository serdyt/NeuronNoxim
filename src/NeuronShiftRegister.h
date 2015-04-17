#ifndef NEURONSHIFTREGISTER_H
#define NEURONSHIFTREGISTER_H

template <class T, int size>
SC_MODULE(ShiftRegister){
public:
	sc_in<T> input;
	sc_out<T> output;
	sc_in_clk clock;

	SC_CTOR(ShiftRegister){
		for (int i = 0; i < size; i++){
			buffer[i] = 0;
		}

		SC_METHOD(shift);
		//set_stack_size(0x5000);
		sensitive << clock.pos();
	}

private:
	T buffer[size];
	void shift();
};

template <class T, int size>
void ShiftRegister<T, size>::shift(){
	//while(true){
	//	wait();

		for (int i = size-1; i > 0; i--){
			buffer[i] = buffer[i-1];
		}
		buffer[0] = input.read();
		output.write(buffer[size-1]);

		/*cout<< sc_time_stamp() << " SR[";
		for (int i = 0; i < size; i++){
			cout<< buffer[i] << ",";
		}
		cout<< "]" <<endl;
		*/
	//}
}

#endif /* NEURONSHIFTREGISTER_H */
