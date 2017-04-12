#include "aes-test.h"
#include <map>
#include <fstream>
#include <iomanip>



using namespace std;

int key_to_hex_buf(unsigned char* key, char* buf, int len)
{
    int index = 0;
    for(int i=0; i< len ;i++)
    {
        index += sprintf(buf+index, "%02x", key[i]);
    }
    index += sprintf(buf+index, "\n");
    return index;
}


int main()
{
    unsigned char *key_text = (unsigned char *)"password1234568";  /* NOT a good password :-) */
    unsigned char key_data[AES_KEY_LENGTH_IN_CHARS];
    unsigned char *clear_text = (unsigned char *)"Four score and seven years ago our fathers brought forth on this continent a new nation, conceived in liberty, and dedicated to the proposition that all men are created equal.";
    int clear_text_len = strlen((const char *)clear_text) + 1; /* add one for null termination */
    
    unsigned char *crypt_text;
    int crypt_text_len;
    unsigned char *clear_crypt_text;
    int clear_crypt_text_len;
    
    AES_KEY enc_key;
    AES_KEY dec_key;
    
    /*
     * Fill in the 128-bit binary key with some text
     * better would be to compute the sha1 has of the text,
     * but this is OK for a class project.
     */
    memset(key_data, 0, sizeof(key_text));
    strncpy((char *)key_data, (char *)key_text, MIN(strlen((const char*)key_text), sizeof((const char*)key_data)));
    /* Now key_data is the 128-bit binary value that AES will use as a key. */
    
    /* test out encryption */
    class_AES_set_encrypt_key(key_data, &enc_key);
    class_AES_encrypt_with_padding(clear_text, clear_text_len, &crypt_text, &crypt_text_len, &enc_key);
    printf("%s\n", crypt_text);
    
    class_AES_set_decrypt_key(key_data, &enc_key);
    class_AES_decrypt_with_padding(crypt_text, crypt_text_len, &clear_crypt_text, &clear_crypt_text_len, &enc_key);
    printf("%s\n", clear_crypt_text);
    
    
    
    unsigned char key[16];
    memset(key, 0,16);
    unsigned char aes_key[16];
    srand ( time(NULL) );
    char key_hex_buf[16];
    memset(key_hex_buf, 0, 16);
    for(int i=0; i< 16; i++)
    {
        key[i] = rand() % 256;
    }
    memset(aes_key, 0, 16);
    memcpy(aes_key, key, 16);
    ofstream ofile;
    ofile.open("test.out");
    memset(aes_key, 0, 16);

    key_to_hex_buf(aes_key, key_hex_buf, 16);
    
    ofile << "this is the hex random key" << endl;
    ofile << hex << key_hex_buf << endl;
    
    map<int,unsigned char*> routerkeys; // routernum is key and AES key is data map<routernum, AES key>

    
    for(int t = 0; t < 5; t++){ // assignes numbers 1 to num
        unsigned char key1[16];
        memset(key1,0,16);
        unsigned char key1cpy[16];
        memset(key1cpy,0,16);
        for(int i=0; i< 16; i++)
        {
            key1[i]=aes_key[i] ^ ((unsigned char)t);
            //cout << hex << key1[i] << endl;
        }
        memcpy(key1cpy, key1, 16);
        for(int i=0; i< 16; i++)
        {
            cout << setw(2) << setfill('0') << hex << key1cpy[i];
        }
        cout << endl;
        routerkeys[t] = key1cpy;
        char out[16];
        memset(out, 0, 16);
        key_to_hex_buf(key1, out, 16);
        ofile << "this is the hex random key with OR" << endl;
        ofile << hex << (char*)out << endl;
        //cout << hex << int(t) << endl;
    }
    
    ofile.close();
    
    
    /* caller must free the buffers */
    free(crypt_text);
    free(clear_crypt_text);
    
    exit(0);
}