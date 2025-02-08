import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from datetime import datetime
import json
import eval
import read
from collections import Counter


def plot_single_cdf(errors, title):
    
    sorted_errors = np.sort(errors)
    y = np.arange(1, len(sorted_errors) + 1) / len(sorted_errors)

    cl_50 = np.percentile(errors, 50)
    cl_95 = np.percentile(errors, 95)

    dpi = 100  # Auflösung in dots per inch
    width_px, height_px = 800, 500 # gewünschte Größe in Pixeln
    figsize = (width_px / dpi, height_px / dpi)

    plt.figure(figsize=figsize, dpi=dpi)
    plt.plot(sorted_errors, y, marker='o', linestyle='-', markersize=5, label='CDF', color = 'orange')

    # Plot vertical lines for 50% and 95% confidence levels
    plt.axvline(x=cl_50, color='b', linestyle='-', label=f'50% Confidence Level: {cl_50:.2f}m')
    plt.axvline(x=cl_95, color='g', linestyle='-', label=f'95% Confidence Level: {cl_95:.2f}m')

    plt.title(title)
    plt.xlabel('Positionierungsfehler in Meter')
    plt.ylabel('Kumulative Wahrscheinlichkeit')
    
    plt.legend(loc='lower right')
    plt.grid(True)
    plt.tight_layout()
   

    print(f"{title}")
    print(f"50% Confidence Level: {cl_50:.2f} meters")
    print(f"95% Confidence Level: {cl_95:.2f} meters")
    
    plt.show()
def plot_multiple_cdfs(errors_list, labels, title):
    """
    Plottet die CDFs von bis zu vier Fehlerdatensätzen in einem einzigen Diagramm.
    
    :param errors_list: Liste der Fehlerdatensätze (z.B. [errors1, errors2, errors3, errors4])
    :param labels: Liste der Labels für die Fehlerdatensätze
    :param title: Titel des Plots
    """
    dpi = 100  # Auflösung in dots per inch
    width_px, height_px = 800, 500 # gewünschte Größe in Pixeln
    figsize = (width_px / dpi, height_px / dpi)

    plt.figure(figsize=figsize, dpi=dpi)

    for errors, label in zip(errors_list, labels):
        sorted_errors = np.sort(errors)
        y = np.arange(1, len(sorted_errors) + 1) / len(sorted_errors)

        plt.plot(
            sorted_errors, y, marker='o', linestyle='-', markersize=5, label=f'{label}'
        )

    # Beschränke die x-Achse auf 150
    

    # Achsenbeschriftungen und Titel
    plt.title(title)
    plt.xlabel('Positionierungsfehler in Meter')
    plt.ylabel('Kumulative Wahrscheinlichkeit')
  
    # Legende
    plt.legend(loc='lower right')

    # Gitter hinzufügen
    plt.grid(True)

    # Layout verbessern
    plt.tight_layout()

    # Diagramm anzeigen
    plt.show()
def plot_coordinates(dataset, title):
    """
    Plottet die Koordinaten des Datasets auf einem Streudiagramm, wobei jeder Punkt eine fortlaufende Nummer erhält.
    
    :param dataset: DataFrame mit den Koordinaten (Longitude, Latitude)
    :param title: Titel des Plots
    """
    plt.figure(figsize=(10, 6))

    # Plot der Longitude und Latitude als Scatter Plot
    #plt.scatter(dataset['Longitude'], dataset['Latitude'], c='blue', marker='o', label='Gemessene Punkte', s=10)
    plt.plot(dataset['Longitude'], dataset['Latitude'], c='orange', linestyle='-', linewidth=1, label="")

    # Titel und Gitter hinzufügen
    plt.title(title)
    plt.grid(True)

    # Entfernen der Achsenticks und -beschriftungen
    plt.xticks([])  # Keine Ticks auf der X-Achse
    plt.yticks([])  # Keine Ticks auf der Y-Achse
    plt.xlabel('')  # Keine X-Achsenbeschriftung
    plt.ylabel('')  # Keine Y-Achsenbeschriftung

    # Füge fortlaufende Nummern für die Punkte hinzu
    #for i, (lon, lat) in enumerate(zip(dataset['Longitude'], dataset['Latitude']), 1):
    #    plt.text(lon, lat, str(i), fontsize=8, color='black', ha='right', va='bottom')

    # Legende
    plt.legend(loc='best')

    # Diagramm anzeigen
    plt.tight_layout()
    plt.show()
def plot_two_datasets(set1, set2, set1_label, set2_label, title):
    """
    Plottet zwei Datensätze (z.B. zwei Routen) im gleichen Fenster.
    
    :param set1: DataFrame mit den interpolierten Koordinaten für die erste Route
    :param set2: DataFrame mit den interpolierten Koordinaten für die zweite Route
    :param set1_label: Label für die erste Route (z.B. "Route 1")
    :param set2_label: Label für die zweite Route (z.B. "Route 2")
    :param title: Titel des Plots
    """
    plt.figure(figsize=(10, 6))

    # Plot der ersten Route
    #plt.plot(set1['Longitude'], set1['Latitude'], c='orange', linestyle='-', linewidth=2, label=set1_label)
    plt.scatter(set1['Longitude'], set1['Latitude'], c='red', linewidths=2, marker='o', label=f'{set1_label}', s=10)

    # Plot der zweiten Route
    plt.plot(set2['Longitude'], set2['Latitude'], c='green', linestyle='-', linewidth=1, label=set2_label)
    plt.scatter(set2['Longitude'],set2['Latitude'], c='blue', marker='o', label=f'Messdaten {set2_label}', s=10)

    # Titel hinzufügen
    plt.title(title)

    # Entfernen der Achsenticks und -beschriftungen
    plt.xticks([])  # Keine Ticks auf der X-Achse
    plt.yticks([])  # Keine Ticks auf der Y-Achse
    plt.xlabel('')  # Keine X-Achsenbeschriftung

    plt.ylabel('')  # Keine Y-Achsenbeschriftung

    # Gitter hinzufügen
    plt.grid(True)

    # Legende
    plt.legend(loc='lower left')

    # Diagramm anzeigen
    plt.tight_layout()
    plt.show()

def plot_error_histogram_gps(errors, title):
    
    # Zahlen runden
    rounded_data = [int(num) for num in errors]
    
    # Häufigkeiten zählen
    frequency = Counter(rounded_data)
    total_count = sum(frequency.values())
    
    # Prozentwerte berechnen
    percentages = {key: ((value / total_count) * 100)/100 for key, value in frequency.items()}

    # Daten für das Diagramm vorbereiten
     # Daten für das Diagramm vorbereiten
    min_label = min(rounded_data)
    max_label = max(rounded_data)
    labels = list(range(0, max_label + 1))  # Durchgehende X-Achse
    values = [percentages.get(label, 0) for label in labels]  # Fehlende Werte mit 0 auffüllen
    
    # Balkendiagramm erstellen
    plt.bar(labels, values, color='skyblue')
    plt.xlabel('Positionsfehler in Metern')
    plt.ylabel('Relative Häufigkeit ')
    plt.title(title)
    plt.xticks(labels)  # Achsenbeschriftung anpassen
    plt.show()
def plot_error_histogram_other(errors, title, group_size=100):
    # Bereichsgröße definieren und die Bereiche erstellen (z.B. 0-100, 101-200, ...)
    max_value = int(max(errors))  # max_value in int umwandeln
    ranges = list(range(0, max_value + group_size, group_size))

    # Zählen, wie viele Fehler in jeden Bereich fallen
    counts = Counter()

    for error in errors:
        # Bestimmen, in welchen Bereich der Fehler fällt
        for i in range(len(ranges) - 1):
            if ranges[i] <= error < ranges[i + 1]:
                counts[f"{ranges[i]} - {ranges[i + 1] - 1}"] += 1
                break

    # Die Häufigkeiten in Prozent umrechnen
    total_count = sum(counts.values())
    percentages = {key: ((value / total_count) * 100)/100 for key, value in counts.items()}

    # Daten für das Diagramm vorbereiten
    labels = [f"{ranges[i]} - {ranges[i + 1] - 1}" for i in range(len(ranges) - 1)]  # Bereichsbezeichner
    values = [percentages.get(f"{ranges[i]} - {ranges[i + 1] - 1}", 0) for i in range(len(ranges) - 1)]  # Prozentwerte für jedes Intervall
    
    # Balkendiagramm erstellen
    plt.bar(ranges[:-1], values, width=group_size * 0.8, color='skyblue', align='edge')  # Balken zentriert

    # X-Achsen-Beschriftungen nur an den Positionen mit Werten setzen
    plt.xlabel('Positionsfehler in Metern')
    plt.ylabel('Relative Häufigkeit')
    plt.title(title)
    
    # X-Achse nur mit den tatsächlichen Beschriftungen, an denen Werte vorhanden sind
    valid_labels = [ranges[i] for i in range(len(ranges) - 1) if percentages.get(f"{ranges[i]} - {ranges[i + 1] - 1}", 0) > 0]
    plt.xticks(valid_labels)  # Nur gültige Positionen für die X-Achse beschriften
    
    plt.tight_layout()  # Platz für die Achsenbeschriftung schaffen
    plt.show()


#####################cdfs bewegung########################

gps_stadt  = read.read_gps_data('../data/bewegung/outdoorstadt.gps.json')
gps_land  = read.read_gps_data('../data/bewegung/gpsland2.json')
lte_stadt  = read.read_gsm_lte_data('../data/bewegung/outdoorstadt.lte.json')

gt_stadt = read.read_groundtruth('../data/bewegung/outdoorstadt_groundtruth.csv')
gt_land = read.read_groundtruth('../data/bewegung/outdoorland_groundtruth.csv')

gps_stadt_error = eval.get_gps_errors(gps_stadt)
gps_land_error = eval.get_gps_errors(gps_land)
lte_stadt_error = eval.calc_error_gsm_lte_moving(lte_stadt, gt_stadt)

"""
plot_single_cdf(gps_stadt_error, "CDF GPS MOBIL Stadt")
plot_single_cdf(gps_land_error, "CDF GPS MOBIL Land")
plot_single_cdf(lte_stadt_error, "CDF LTE MOBIL Stadt")
"""

walking_gps_error = [gps_stadt_error, gps_land_error,]
walking_gps_errors_labels = ["GPS Stadt", "GPS Land"]
"""
plot_multiple_cdfs(walking_gps_error, walking_gps_errors_labels, "VERGLEICH CDF GPS MOBIL FÜR STADT/LAND")
"""

###################cdfs stationär#########################

gsm_hof = read.read_gsm_lte_data('../data/stationaer/stationaer.gsmhof.json')
gsm_keller= read.read_gsm_lte_data('../data/stationaer/stationaer.gsmkeller.json')
gsm_wohnung = read.read_gsm_lte_data('../data/stationaer/stationaer.gsmwohnung.json')

lte_hof = read.read_gsm_lte_data('../data/stationaer/stationaer.ltehof.json')
lte_keller = read.read_gsm_lte_data('../data/stationaer/stationaer.ltekeller.json')
lte_wohnung = read.read_gsm_lte_data('../data/stationaer/stationaer.ltewohnung.json')

gt_hof = read.read_groundtruth('../data/stationaer/stationaer.hof.groundtruth.csv')
gt_keller = read.read_groundtruth('../data/stationaer/stationaer.keller.groundtruth.csv')
gt_wohnung = read.read_groundtruth('../data/stationaer/stationaer.wohnung.groundtruth.csv')


gsm_hof_error = eval.calc_error_gsm_lte_stationary(gt_hof, gsm_hof)
gsm_keller_error = eval.calc_error_gsm_lte_stationary(gt_keller, gsm_keller)
gsm_wohnung_error = eval.calc_error_gsm_lte_stationary(gt_wohnung, gsm_wohnung)

lte_hof_error = eval.calc_error_gsm_lte_stationary(gt_hof, lte_hof)
lte_keller_error = eval.calc_error_gsm_lte_stationary(gt_keller, lte_keller)
lte_wohnung_error = eval.calc_error_gsm_lte_stationary(gt_wohnung, lte_wohnung)

"""
plot_single_cdf(gsm_hof_error, "CDF GSM STATIONÄR HOF")
plot_single_cdf(gsm_keller_error, "CDF GSM STATIONÄR KELLER")
plot_single_cdf(gsm_wohnung_error, "CDF GSM STATIONÄR WOHNUNG")
plot_single_cdf(lte_hof_error, "CDF LTE STATIONÄR HOF")
plot_single_cdf(lte_keller_error, "CDF LTE STATIONÄR KELLER")
plot_single_cdf(lte_wohnung_error, "CDF LTE STATIONÄR WOHNUNG")
"""


gsm_stationär_errors = [gsm_hof_error,gsm_keller_error, gsm_wohnung_error ]
gsm_stationär_labels = ["GSM HOF", "GSM KELLER", "GSM WOHNUNG"]

lte_stationär_errors = [lte_hof_error,lte_keller_error, lte_wohnung_error ]
lte_stationär_labels = ["LTE HOF", "LTE KELLER", "LTE WOHNUNG"]


gsm_lte_stationär_errors = [gsm_hof_error,gsm_keller_error, gsm_wohnung_error,lte_hof_error,lte_keller_error, lte_wohnung_error ]
gsm_lte_stationär_labels = ["GSM HOF", "GSM KELLER", "GSM WOHNUNG", "LTE HOF", "LTE KELLER", "LTE WOHNUNG"]

"""
plot_multiple_cdfs(gsm_stationär_errors, gsm_stationär_labels, "VERGLEICH CDF GSM STATIONÄR FÜR HOF/KELLER/WOHNUNG")
plot_multiple_cdfs(lte_stationär_errors, lte_stationär_labels, "VERGLEICH CDF LTE STATIONÄR FÜR HOF/KELLER/WOHNUNG")
plot_multiple_cdfs(gsm_lte_stationär_errors, gsm_lte_stationär_labels, "VERGLEICH CDF GSM/LTE STATIONÄR FÜR HOF/KELLER/WOHNUNG ")
"""

###################cdfs autobahn#########################

gps1 = read.read_gps_data('../data/autobahn/autobahn.gps1.json')
gps2 = read.read_gps_data('../data/autobahn/autobahn.gps2.json')
gsm_atb = read.read_gsm_lte_data('../data/autobahn/autobahn.gsm.json')
lte = read.read_gsm_lte_data('../data/autobahn/autobahn.lte.json')

gps1_gt = read.read_groundtruth('../data/autobahn/autobahn.gps1.groundtruth.csv')
gps2_gt = read.read_groundtruth('../data/autobahn/autobahn.gps2.groundtruth.csv')
gsm_atb_gt = read.read_groundtruth('../data/autobahn/autobahn.gsm.groundtruth.csv')
lte_gt = read.read_groundtruth('../data/autobahn/autobahn.lte.groundtruth.csv')

gps1_error = eval.get_gps_errors(gps1)
gps2_error = eval.get_gps_errors(gps2)
gsm_atb_error = eval.calc_error_gsm_lte_moving(gsm_atb, gsm_atb_gt)
lte_atb_error = eval.calc_error_gsm_lte_moving(lte, lte_gt)

"""

plot_single_cdf(gps1_error, "CDF GPS1 MOBIL AUTOBAHN")
plot_single_cdf(gps2_error, "CDF GPS2 MOBIL AUTOBAHN")
plot_single_cdf(gsm_error, "CDF GSM MOBIL AUTOBAHN")
plot_single_cdf(lte_error, "CDF LTE MOBIL AUTOBAHN")
"""

gps_errors=[gps1_error, gps2_error]
gps_errors_labels=["GPS1","GPS2"]

cell_errors=[gsm_atb_error, lte_atb_error]
cell_errors_labels=["GSM","LTE"]
"""
plot_multiple_cdfs(gps_errors, gps_errors_labels, "VERGLEICH CDF GPS FÜR AUTOBAHN")
plot_multiple_cdfs(cell_errors, cell_errors_labels, "VERGLEICH CDF GPS/LTE FÜR AUTOBAHN")
"""

###########cdfs alle gps daten outdoor daten############

all_gps_errors = [gps_stadt_error, gps_land_error, gps1_error, gps2_error]
all_gps_errors_labels = ["GPS Stadt", "GPS Land", "GPS Autobahn 1", "GPS Autobahn 2"]
"""
plot_multiple_cdfs(all_gps_errors, all_gps_errors_labels, "VERGLEICH CDF GPS MOBIL FÜR STADT/LAND/AUTOBAHN")
"""
##########histogramme gps##################
"""
plot_error_histogram_gps(gps_stadt_error, "FEHLER HISTOGRAMM GPS STADT")
plot_error_histogram_gps(gps_land_error, "FEHLER HISTOGRAMM GPS LAND")
plot_error_histogram_gps(gps1_error, "FEHLER HISTOGRAMM GPS1 AUTOBAHN")
plot_error_histogram_gps(gps2_error, "FEHLER HISTOGRAMM GPS2 AUTOBAHN")
"""

###############histgoramme gsm_lte#############
"""
plot_error_histogram_other(gsm_atb_error, "FEHLER HISTOGRAMM GSM AUTOBAHN")
plot_error_histogram_other(lte_atb_error, "FEHLER HISTOGRAMM LTE AUTOBAHN", group_size= 300)
plot_error_histogram_other(lte_stadt_error, "FEHLER HISTOGRAMM LTE STADT")
plot_error_histogram_other(gsm_hof_error, "FEHLER HISTOGRAMM GSM HOF")
plot_error_histogram_other(gsm_keller_error, "FEHLER HISTOGRAMM GSM KELLER")
plot_error_histogram_other(gsm_wohnung_error, "FEHLER HISTOGRAMM GSM WOHNUNG")
plot_error_histogram_other(lte_hof_error, "FEHLER HISTOGRAMM LTE HOF")
plot_error_histogram_other(lte_keller_error, "FEHLER HISTOGRAMM LTE KELLER")
plot_error_histogram_other(lte_wohnung_error, "FEHLER HISTOGRAMM LTE WOHNUNG")
"""
#############routen###################
#plot_two_datasets(gsm_atb, gsm_atb_gt, "GSM", "Groundtruth", "VERGLEICH GSM MESSDATEN UND GROUNDTRUTH AUTOBAHN")
############more cdfs#################
"""
gpshof = read.read_gps_data('../data/stationaer/gpshof.json')
gpshoferror = eval.get_gps_errors(gpshof)
plot_single_cdf(gpshoferror, "CDF GPS STATIONAER HOF")
plot_error_histogram_gps(gpshoferror, "FEHLER HISTOGRAMM GPS HOF")
"""

gpswohnung = read.read_gps_data('../data/stationaer/gpswohnung.json')
gpswohnungerror = eval.get_gps_errors(gpswohnung)
#plot_single_cdf(gpswohnungerror, "CDF GPS STATIONAER WOHNUNG")
#plot_error_histogram_other(gpswohnungerror, "FEHLER HISTOGRAMM GPS WOHNUNG")




gsmstadt = read.read_gsm_lte_data('../data/bewegung/gsmstadt.json')
gssgt = read.read_groundtruth('../data/bewegung/gsmstadt.csv')
gsmstadterror = eval.calc_error_gsm_lte_moving(gsmstadt, gssgt)
#plot_single_cdf(gsmstadterror, "CDF GSM MOBIL STADT")
#plot_error_histogram_other(gsmstadterror, "FEHLER HISTOGRAMM GSM STADT")

gsmland = read.read_gsm_lte_data('../data/bewegung/gsmland.json')
gsmltegps_gt = read.read_groundtruth('../data/bewegung/gsmltegps.csv')

gsmlanderror = eval.calc_error_gsm_lte_moving(gsmland, gsmltegps_gt)
#plot_single_cdf(gsmlanderror, 'CDF GSM MOBIL LAND')
#plot_error_histogram_other(gsmlanderror, "FEHLER HISTOGRAMM GSM LAND")



lteland = read.read_gsm_lte_data('../data/bewegung/lteland.json')
ltelanderror = eval.calc_error_gsm_lte_moving(lteland, gsmltegps_gt)
ltelanderror = ltelanderror[:9]
print(len(ltelanderror))
#plot_single_cdf(ltelanderror, 'CDF LTE MOBIL LAND')
#plot_error_histogram_other(ltelanderror, "FEHLER HISTOGRAMM LTE LAND")

"""
nbiot = read.read_gsm_lte_data('../data/nbiot/nbiot.json')
####moving nicht möglich, da keine daten gesendet#####also stationär######
nbiote = eval.calc_error_gsm_lte_stationary(gt_wohnung, nbiot)
#plot_single_cdf(nbiote, "CDF NBIOT STATIONÄR WOHNUNG")
plot_error_histogram_other(nbiote, "FEHLER HISTOGRAMM NBIOT WOHNUNG")
"""
"""
walking_gsm_error = [gsmstadterror, gsmlanderror,]
walking_gsm_errors_labels = ["GSM Stadt", "GSM Land"]
plot_multiple_cdfs(walking_gsm_error, walking_gsm_errors_labels, "VERGLEICH CDF GSM MOBIL FÜR STADT/LAND")

walking_lte_error = [lte_stadt_error, ltelanderror,]
walking_lte_errors_labels = ["LTE Stadt", "LTE Land"]

mobileerrors = [gsmstadterror, gsmlanderror,lte_stadt_error, ltelanderror]
mobileelabels = ["GSM Stadt", "GSM Land", "LTE Stadt", "LTE Land"]
plot_multiple_cdfs(mobileerrors, mobileelabels, "VERGLEICH CDF GSM/LTE MOBIL FÜR STADT/LAND")


walkginge = [gsmlanderror, ltelanderror]
mobileelabels= ["GSM Land", "LTE Land"]
plot_multiple_cdfs(walkginge, mobileelabels, "VERGLEICH CDF GSM/LTE MOBIL FÜR Land")

gpserorrs2 = [gps_stadt_error, gps_land_error, gps1_error, gps2_error]
gpserorrs2labels= ["GPS STADT", "GPS LAND", "GPS AUTOBAHN 1", "GPS AUTOBAHN2"]
plot_multiple_cdfs(gpserorrs2, gpserorrs2labels, title="VERGLEICH CDF GPS FÜR STADT/LAND/AUTOBAHN")


plot_error_histogram_gps(gps_land_error, "FEHLER HISTOGRAMM GPS LAND")

allmovementerr= [gps1_error, gps2_error, gsm_atb_error,lte_atb_error]
ee = ["GPS1 AUTOBAHN", "GPS2 AUTOBAHN","GSM AUTOBAHN", "LTE AUTOBAHN" ]
plot_multiple_cdfs(allmovementerr, ee, "VERGLEICH CDF GPS/GSM/LTE FÜR AUTOBAHN")
"""


"""
stadt = read.read_groundtruth('../data/bewegung/stadtgt.csv')
plot_coordinates(stadt, "GROUNDTRUTH STADT")
plot_two_datasets(stadt, gsmstadt, "GROUNDTRUTH STADT", "GSM", "VERGLEICH GROUNDTRUTH STADT UND GSM")
plot_two_datasets(stadt, lte_stadt, "GROUNDTRUTH STADT", "LTE", "VERGLEICH GROUNDTRUTH STADT UND LTE")
plot_two_datasets(stadt, gps_stadt, "GROUNDTRUTH STADT", "GPS", "VERGLEICH GROUNDTRUTH STADT UND GPS")
"""
"""
lteab = read.read_groundtruth('../data/autobahn/autobahn.lte.groundtruth.csv')
gsmab = read.read_groundtruth('../data/autobahn/autobahn.gsm.groundtruth.csv')
gps1ab = read.read_groundtruth('../data/autobahn/autobahn.gps1.groundtruth.csv')
gps2ab = read.read_groundtruth('../data/autobahn/autobahn.gps2.groundtruth.csv')


plot_two_datasets(gps1ab, gps1, "GROUNDTRUTH AUTOBAHN 1", "GPS", "VERGLEICH GROUNDTRUTH AUTOBAHN UND GPS 1")
plot_two_datasets(gps2ab, gps2, "GROUNDTRUTH AUTOBAHN 2", "GPS", "VERGLEICH GROUNDTRUTH AUTOBAHN UND GPS 2")

plot_two_datasets(gsmab, gsm_atb, "GROUNDTRUTH AUTOBAHN", "GSM", "VERGLEICH GROUNDTRUTH AUTOBAHN UND GSM")
plot_two_datasets(lteab, lte, "GROUNDTRUTH AUTOBAHN", "LTE", "VERGLEICH GROUNDTRUTH AUTOBAHN UND LTE")
"""
gpshof = read.read_gps_data('../data/stationaer/gpshof.json')

"""
plot_two_datasets(gt_hof, gsm_hof, "GROUNDTRUTH HOF", "GSM", "VERGLEICH GROUNDTRUTH HOF UND GSM")
plot_two_datasets(gt_hof, lte_hof, "GROUNDTRUTH HOF", "LTE", "VERGLEICH GROUNDTRUTH HOF UND LTE")
plot_two_datasets(gt_hof, gpshof, "GROUNDTRUTH HOF", "GPS", "VERGLEICH GROUNDTRUTH HOF UND GPS")
"""
"""
gpswo = read.read_gps_data('../data/stationaer/gpswohnung.json')
plot_two_datasets(gt_wohnung, gsm_wohnung, "GROUNDTRUTH WOHNUNG", "GSM", "VERGLEICH GROUNDTRUTH WOHNUNG UND GSM")
plot_two_datasets(gt_wohnung, lte_wohnung, "GROUNDTRUTH WOHNUNG", "LTE", "VERGLEICH GROUNDTRUTH WOHNUNG UND LTE")
plot_two_datasets(gt_wohnung, gpswo, "GROUNDTRUTH WOHNUNG", "GPS", "VERGLEICH GROUNDTRUTH WOHNUNG UND GPS")
"""
#plot_two_datasets(gt_keller, gsm_keller, "GROUNDTRUTH KELLER", "GSM", "VERGLEICH GROUNDTRUTH KELLER UND GSM")
#plot_two_datasets(gt_keller, lte_keller, "GROUNDTRUTH KELLER", "LTE", "VERGLEICH GROUNDTRUTH KELLER UND LTE")
#plot_two_datasets(gt_keller, gpswo, "GROUNDTRUTH KELLER", "GPS", "VERGLEICH GROUNDTRUTH KELLER UND GPS")

print(gps_stadt)
plot_gps_data(gps_land)

plot_other_data()







