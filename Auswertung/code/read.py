import json
import pandas as pd
import xml.etree.ElementTree as ET
from bs4 import BeautifulSoup

def read_groundtruth(file_path):
    df = pd.read_csv(file_path, delimiter=';')
    if 'Timestamp' in df.columns:
        df['Timestamp'] = pd.to_datetime(df['Timestamp'])
    return df

def read_geotracker(file_path, csvtitle):
    with open(file_path, 'r') as file:
        data = file.read()

    Bs_data = BeautifulSoup(data, "xml")
    measurements = Bs_data.find_all('trkpt')

    latitudes = []
    longitudes = []
    timestamps = []

    coordinates = []

    for trkpt in measurements:
        lat = trkpt.get('lat')
        lon = trkpt.get('lon')
        time = trkpt.find('time').text if trkpt.find('time') else None

        if lat and lon and time:
            latitudes.append(float(lat))
            longitudes.append(float(lon))
            timestamps.append(time)

    df = pd.DataFrame({
        'Timestamp': timestamps,
        'Latitude': latitudes,
        'Longitude': longitudes
    })

    #df.to_csv(csvtitle, sep=";", columns=["Timestamp", "Latitude","Longitude"], index=None,  )
    return df
    
def read_gps_data(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
    
    # Erstelle eine Liste von Dictionnaries mit den gewünschten Feldern
    extracted_data = []
    for entry in data:
        # Extrahiere die relevanten Felder
        extracted_data.append({
            'Mode': entry.get('mode'),
            'Timestamp': entry.get('createdAt'),
            'Latitude': entry.get('latitude'),
            'Longitude': entry.get('longitude'),
            'Error': entry.get('accuracy'),
            'Satellites': entry.get('nsat'),
            'Hdop': entry.get('hdop'),
            'TTFF' : entry.get('timeToGetFirstFix'),
            
        })
    df = pd.DataFrame(extracted_data)
    

    # df.to_csv('', sep=";", columns=["Latitude","Longitude"], index=None,)
    return df

def read_gsm_lte_data(file_path):
    # Öffne und lade die JSON-Datei
    with open(file_path, 'r') as file:
        data = json.load(file)
    
    # Erstelle eine Liste von Dictionnaries mit den gewünschten Feldern
    extracted_data = []
    for entry in data:
        # Extrahiere die relevanten Felder
        extracted_data.append({
            'Mode': entry.get('mode'),
            'Timestamp': entry.get('createdAt'),
            'Latitude': entry.get('latitude'),
            'Longitude': entry.get('longitude'),
            'Accuracy': entry.get('accuracy'),
            'Cells': entry.get('ncells'),
            'Signal': entry.get('avg_signal')
        })
    df = pd.DataFrame(extracted_data)
    #df.to_csv('', sep=";", columns=["Latitude","Longitude"], index=None,)
    return df

if __name__ == '__main__':
   #df = read_geotracker('../data/outdoorstadt_groundtruth.gpx','outdoorstadt_groundtruth.csv')
  pass

