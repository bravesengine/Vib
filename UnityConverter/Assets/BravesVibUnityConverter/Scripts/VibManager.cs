using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;
using System.Text.RegularExpressions;

public class VibManager : MonoBehaviour
{
    [System.Serializable]
    public class VibDataContent
    {
        public string HeaderDesc;
        public string RawBase64;
        public string RawData;
        public int Size;
        public int Version;
    }

    public static VibManager instance; 
    public static Dictionary<VibKey, OVRHapticsClip>  vibDictionary;
    public static VibKey testKey = (VibKey) 0;

    private void Awake()
    {
        if(instance != null)
        {
            Destroy(this);
        }
        instance = this;
    }

    private void Start()
    {
        vibDictionary = new Dictionary<VibKey, OVRHapticsClip>();
        ReadVibDataFromJson();
    }


    //Read RawData from each json files in vibDataFiles array and add those data to vibDictionary
    private void ReadVibDataFromJson()
    {
        TextAsset[] _vibDataFiles = Resources.LoadAll<TextAsset>("VibData");

        for(int _i = 0 ; _i < _vibDataFiles.Length ; _i ++)
        {
            VibDataContent vibDataContent = JsonUtility.FromJson<VibDataContent>(_vibDataFiles[_i].ToString());  //parse json
            string[] strigns = vibDataContent.RawData.Split(',');
            byte[] _bytes = new byte[strigns.Length];             //convert rawdata to byte array
            for(int i = 0; i < strigns.Length; i++)
            {
                _bytes[i] = System.Convert.ToByte(int.Parse(strigns[i]));
            }

            OVRHapticsClip _clip = new OVRHapticsClip(_bytes, _bytes.Length);

            string _name = _vibDataFiles[_i].name;
            _name = _name = Regex.Replace(_name, @"[^a-zA-Z0-9가-힣]", "_"); 
            _name = _name.Replace(" ", string.Empty); 
            VibKey _key =(VibKey) System.Enum.Parse(typeof(VibKey), _name);
            vibDictionary.Add(_key, _clip);               //add these data sets to dictionary
        }        
    }

    private void ReadVibDataFromAudioClips()
    {
        AudioClip[] _audioClips = Resources.LoadAll<AudioClip>("VibData");

        for(int _i = 0 ; _i < _audioClips.Length ; _i ++)
        {
            OVRHapticsClip _clip = new OVRHapticsClip(_audioClips[_i]);

            string _name = _audioClips[_i].name;
            _name = _name = Regex.Replace(_name, @"[^a-zA-Z0-9가-힣]", "_"); 
            _name = _name.Replace(" ", string.Empty);             
            VibKey _key =(VibKey) System.Enum.Parse(typeof(VibKey), _name);
            vibDictionary.Add(_key, _clip);               //add these data sets to dictionary            
        }
    }


    // String to byte array
    private byte[] StringToByte(string str)
    {
        string[] strigns = str.Split(',');

        byte[] bytes = new byte[strigns.Length];             //convert rawdata to byte array
        for(int i = 0; i < strigns.Length; i++)
        {
            bytes[i] = System.Convert.ToByte(int.Parse(strigns[i]));
        }

        //byte[] StrByte = System.Text.Encoding.UTF8.GetBytes(str);
        //string returnStr = System.Text.Encoding.Default.GetString(StrByte);

        Debug.Log(bytes);

        return bytes;
    }

    // byte array to string
    private string ByteToString(byte[] strByte)
    {
        string str = System.Text.Encoding.Default.GetString(strByte);
        return str;
    }

}
