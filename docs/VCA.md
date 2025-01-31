### **.vca Files**  

Trace files with a **.vca** extension contain a list of memory access operations. Each file consists of at least **three** required fields, with up to **five** possible fields per entry.  

---

### **File Structure**  
Each entry in the **.vca** file follows this structure:  

1. **Access Type:**  
   - **L** → Read operation  
   - **S** → Write operation  

2. **Memory Address:**  
   - Represented in **hexadecimal** format, always preceded by `0x`.  
   - Example: `0x08000100`  

3. **Instruction/Data Identifier:**  
   - **I** → Instruction address  
   - **D** → Data address  

4. **Word Count (Optional):**  
   - Specifies the number of words accessed per operation.  
   - If omitted, the default value is used.  

5. **Write Data (Only for `S` Operations, Optional):**  
   - The **decimal** value to be written to memory.  
   - Only applies to **write (`S`)** operations.  

---

### **Additional Features**  

- **Comments:**  
  - Lines starting with a `#` are treated as comments and ignored by the simulator.  

- **Breakpoints:**  
  - Any instruction can be preceded by `!` to mark it as a **breakpoint**.  

---

### **Example Entry**  
```
L 0x08000100 I 4        # Read 4 words from an instruction address  
S 0x08000200 D 1 255    # Write 255 to a data address  
! L 0x08000300 D 2      # Breakpoint: Read 2 words from a data address  
```
